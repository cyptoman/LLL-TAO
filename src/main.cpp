/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/


#include <LLP/include/global.h>
#include <LLP/include/port.h>
#include <LLP/types/apinode.h>
#include <LLP/types/rpcnode.h>
#include <LLP/types/legacy_miner.h>
#include <LLP/types/tritium_miner.h>
#include <LLP/include/lisp.h>

#include <LLD/include/global.h>

#include <TAO/API/include/global.h>
#include <TAO/API/include/cmd.h>
#include <TAO/Ledger/include/create.h>
#include <TAO/Ledger/include/chainstate.h>
#include <TAO/Ledger/types/tritium_minter.h>

#include <Util/include/convert.h>
#include <Util/include/filesystem.h>
#include <Util/include/signals.h>
#include <Util/include/daemon.h>

#include <Legacy/include/ambassador.h>
#include <Legacy/types/legacy_minter.h>
#include <Legacy/wallet/wallet.h>

#ifndef WIN32
#include <sys/resource.h>
#endif

int main(int argc, char** argv)
{

    /* Setup the timer timer. */
    runtime::timer timer;
    timer.Start();


    /* Handle all the signals with signal handler method. */
    SetupSignals();


    /* Read the configuration file. Pass argc and argv for possible -datadir setting */
    config::ReadConfigFile(config::mapArgs, config::mapMultiArgs, argc, argv);


    /* Parse out the parameters */
    config::ParseParameters(argc, argv);


    /* Once we have read in the CLI paramters and config file, cache the args into global variables*/
    config::CacheArgs();


    /** Initialize network resources. (Need before RPC/API for WSAStartup call in Windows) **/
    if(!LLP::Initialize())
    {
        printf("ERROR: Failed initializing network resources");

        return 0;
    }


    /* Handle Commandline switch */
    for(int i = 1; i < argc; ++i)
    {
        if(!convert::IsSwitchChar(argv[i][0]))
        {
            int nRet = 0;

            /* As a helpful shortcut, if the method name includes a "/" then we will assume it is meant for the API
               since none of the RPC commands support a "/" in the method name */
            bool fIsAPI = false;

            std::string endpoint = std::string(argv[i]);
            std::string::size_type pos = endpoint.find('/');
            if(pos != endpoint.npos || config::GetBoolArg(std::string("-api")))
                fIsAPI = true;

            if(fIsAPI)
                nRet = TAO::API::CommandLineAPI(argc, argv, i);
            else
                nRet = TAO::API::CommandLineRPC(argc, argv, i);

            return nRet;
        }
    }


    /* Log system startup now, after branching to API/RPC where appropriate */
    debug::Initialize();


    /** Run the process as Daemon RPC/LLP Server if Flagged. **/
    if(config::fDaemon)
    {
        debug::log(0, FUNCTION, "-daemon flag enabled. Running in background");
        Daemonize();
    }


    /* Create directories if they don't exist yet. */
    if(!filesystem::exists(config::GetDataDir()) &&
        filesystem::create_directory(config::GetDataDir()))
    {
        debug::log(0, FUNCTION, "Generated Path ", config::GetDataDir());
    }

    /** Handle the beta server. */
    uint16_t nPort = 0;
    if(!config::GetBoolArg(std::string("-beta")))
    {
        /* Get the port for Tritium Server. */
        nPort = static_cast<uint16_t>(config::GetArg(std::string("-port"), config::fTestNet.load() ? (TRITIUM_TESTNET_PORT + (config::GetArg("-testnet", 0) - 1)) : TRITIUM_MAINNET_PORT));

        /* Initialize the Tritium Server. */
        LLP::TRITIUM_SERVER = LLP::CreateTAOServer<LLP::TritiumNode>(nPort);
    }
    else
    {
        /* Get the port for Legacy Server. */
        nPort = static_cast<uint16_t>(config::GetArg(std::string("-port"), config::fTestNet.load() ? (LEGACY_TESTNET_PORT + (config::GetArg("-testnet", 0) - 1)) : LEGACY_MAINNET_PORT));

        /* Initialize the Legacy Server. */
        LLP::LEGACY_SERVER = LLP::CreateTAOServer<LLP::LegacyNode>(nPort);
    }

    nPort = static_cast<uint16_t>(config::fTestNet.load() ? TESTNET_CORE_LLP_PORT : MAINNET_CORE_LLP_PORT);

    /** Startup the time server. **/
    LLP::TIME_SERVER = new LLP::Server<LLP::TimeNode>(
        nPort,
        10,
        30,
        false,
        0,
        0,
        10,
        config::GetBoolArg(std::string("-unified"), false),
        config::GetBoolArg(std::string("-meters"), false),
        true,
        30000);

    /* Startup timer stats. */
    uint32_t nElapsed = 0;

    /* Check for failures. */
    bool fFailed = config::fShutdown.load();
    if(!fFailed)
    {
        /* Initialize LLD. */
        LLD::Initialize();

        /** Load the Wallet Database. **/
        bool fFirstRun;
        if (!Legacy::Wallet::InitializeWallet(config::GetArg(std::string("-wallet"), Legacy::WalletDB::DEFAULT_WALLET_DB)))
            return debug::error("Failed initializing wallet");


        /** Check the wallet loading for errors. **/
        uint32_t nLoadWalletRet = Legacy::Wallet::GetInstance().LoadWallet(fFirstRun);
        if (nLoadWalletRet != Legacy::DB_LOAD_OK)
        {
            if (nLoadWalletRet == Legacy::DB_CORRUPT)
                return debug::error("Failed loading wallet.dat: Wallet corrupted");
            else if (nLoadWalletRet == Legacy::DB_TOO_NEW)
                return debug::error("Failed loading wallet.dat: Wallet requires newer version of Nexus");
            else
                return debug::error("Failed loading wallet.dat");
        }


        /** Rebroadcast transactions. **/
        Legacy::Wallet::GetInstance().ResendWalletTransactions();


        /** Initialize ChainState. */
        TAO::Ledger::ChainState::Initialize();


        /** Initialize the scripts for legacy mode. **/
        Legacy::InitializeScripts();


        /** Handle Rescanning. **/
        if(config::GetBoolArg(std::string("-rescan")))
            Legacy::Wallet::GetInstance().ScanForWalletTransactions(&TAO::Ledger::ChainState::stateGenesis, true);


        /* Initialize API Pointers. */
        TAO::API::Initialize();

        /* Get the port for the Core API Server. */
        nPort = static_cast<uint16_t>(config::GetArg(std::string("-apiport"), config::fTestNet.load() ? TESTNET_API_PORT : MAINNET_API_PORT));

        /* Create the Core API Server. */
        LLP::API_SERVER = new LLP::Server<LLP::APINode>(
            nPort,
            10,
            30,
            false,
            0,
            0,
            60,
            true,
            false,
            false);


        /* Get the port for the Core API Server. */
        nPort = static_cast<uint16_t>(config::GetArg(std::string("-rpcport"), config::fTestNet.load() ? TESTNET_RPC_PORT : MAINNET_RPC_PORT));

        /* Set up RPC server */
        LLP::RPC_SERVER = new LLP::Server<LLP::RPCNode>(
            nPort,
            static_cast<uint16_t>(config::GetArg(std::string("-rpcthreads"), 4)),
            30,
            false,
            0,
            0,
            60,
            config::GetBoolArg("-listen", true),
            false,
            false);


        /* Handle Manual Connections from Command Line, if there are any. */
        if(config::GetBoolArg("-beta"))
            LLP::MakeConnections<LLP::LegacyNode>(LLP::LEGACY_SERVER);
        else
            LLP::MakeConnections<LLP::TritiumNode>(LLP::TRITIUM_SERVER);


        /* Set up Mining Server */
        if(config::GetBoolArg(std::string("-mining")))
        {
          if(config::GetBoolArg(std::string("-beta")))
              LLP::LEGACY_MINING_SERVER  = LLP::CreateMiningServer<LLP::LegacyMiner>();
          else
              LLP::TRITIUM_MINING_SERVER = LLP::CreateMiningServer<LLP::TritiumMiner>();
        }


        /* Elapsed Milliseconds from timer. */
        nElapsed = timer.ElapsedMilliseconds();
        timer.Stop();


        /* If wallet is not encrypted, it is unlocked by default. Start stake minter now. It will run until stopped by system shutdown. */
        if (!Legacy::Wallet::GetInstance().IsCrypted())
            Legacy::LegacyMinter::GetInstance().Start();


        /* Startup performance metric. */
        debug::log(0, FUNCTION, "Started up in ", nElapsed, "ms");


        /* Initialize generator thread. */
        std::thread thread;
        if(config::GetBoolArg(std::string("-private")))
            thread = std::thread(TAO::Ledger::ThreadGenerator);


        /* Wait for shutdown. */
        if(!config::GetBoolArg(std::string("-gdb")))
        {
            std::mutex SHUTDOWN_MUTEX;
            std::unique_lock<std::mutex> SHUTDOWN_LOCK(SHUTDOWN_MUTEX);
            SHUTDOWN.wait(SHUTDOWN_LOCK, []{ return config::fShutdown.load(); });
        }


        /* GDB mode waits for keyboard input to initiate clean shutdown. */
        else
        {
            getchar();
            config::fShutdown = true;
        }


        /* Stop stake minter if it is running (before server shutdown). */
        if(config::GetBoolArg(std::string("-beta")))
            Legacy::LegacyMinter::GetInstance().Stop();
        else
            TAO::Ledger::TritiumMinter::GetInstance().Stop();


        /* Wait for the private condition. */
        if(config::GetBoolArg(std::string("-private")))
        {
            TAO::Ledger::PRIVATE_CONDITION.notify_all();
            thread.join();
        }
    }


    /* Shutdown metrics. */
    timer.Reset();


    /* Shutdown the time server and its subsystems. */
    LLP::ShutdownServer<LLP::TimeNode>(LLP::TIME_SERVER);


    /* Shutdown the tritium server and its subsystems. */
    LLP::ShutdownServer<LLP::TritiumNode>(LLP::TRITIUM_SERVER);


    /* Shutdown the legacy server and its subsystems. */
    LLP::ShutdownServer<LLP::LegacyNode>(LLP::LEGACY_SERVER);


    /* Shutdown the core API server and its subsystems. */
    LLP::ShutdownServer<LLP::APINode>(LLP::API_SERVER);


    /* Shutdown the RPC server and its subsystems. */
    LLP::ShutdownServer<LLP::RPCNode>(LLP::RPC_SERVER);


    /* Shutdown the legacy mining server and its subsystems. */
    LLP::ShutdownServer<LLP::LegacyMiner>(LLP::LEGACY_MINING_SERVER);


    /* Shutdown the tritium mining server and its subsystems. */
    LLP::ShutdownServer<LLP::TritiumMiner>(LLP::TRITIUM_MINING_SERVER);


    /* Shutdown the API. */
    TAO::API::Shutdown();


    /* After all servers shut down, clean up underlying networking resources */
    LLP::Shutdown();


    /* Shutdown database instances. */
    LLD::Shutdown();


    /* Shutdown these subsystems if nothing failed. */
    if(!fFailed)
    {
        /* Shut down wallet database environment. */
        if (config::GetBoolArg(std::string("-flushwallet"), true))
            Legacy::WalletDB::ShutdownFlushThread();

        Legacy::BerkeleyDB::GetInstance().Shutdown();
    }


    /* Elapsed Milliseconds from timer. */
    nElapsed = timer.ElapsedMilliseconds();


    /* Startup performance metric. */
    debug::log(0, FUNCTION, "Closed in ", nElapsed, "ms");


    /* Close the debug log file once and for all. */
    debug::Shutdown();

    return 0;
}
