/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#pragma once
#ifndef NEXUS_LLP_TYPES_LEGACY_H
#define NEXUS_LLP_TYPES_LEGACY_H

#include <Legacy/types/legacy.h>
#include <TAO/Ledger/types/locator.h>

#include <LLP/include/legacy_address.h>
#include <LLP/include/network.h>
#include <LLP/include/version.h>
#include <LLP/packets/legacy.h>
#include <LLP/templates/base_connection.h>
#include <LLP/templates/ddos.h>
#include <LLP/templates/events.h>

namespace LLP
{

    /** LegacyNode
     *
     *  A Node that processes packets and messages for the Legacy Server
     *
     **/
    class LegacyNode : public BaseConnection<LegacyPacket>
    {
        /** Mutex to protect connected sessions. **/
        static std::mutex SESSIONS_MUTEX;


        /** Map to keep track of duplicate nonce sessions. **/
        static std::map<uint64_t, std::pair<uint32_t, uint32_t>> mapSessions;


        /** Switch Node
         *
         *  Helper function to switch available nodes.
         *
         **/
        static void SwitchNode();


    public:

        /** Name
         *
         *  Returns a string for the name of this type of Node.
         *
         **/
        static std::string Name() { return "Legacy"; }


        /** Default Constructor **/
        LegacyNode();


        /** Constructor **/
        LegacyNode(Socket SOCKET_IN, DDOS_Filter* DDOS_IN, bool isDDOS = false);


        /** Constructor **/
        LegacyNode(DDOS_Filter* DDOS_IN, bool isDDOS = false);


        /* Virtual destructor. */
        virtual ~LegacyNode();


        /** String version of this Node's Version. **/
        std::string strNodeVersion;


        /** The current Protocol Version of this Node. **/
        uint32_t nCurrentVersion;


        /** The current session ID. **/
        uint64_t nCurrentSession;


        /** LEGACY: The height of this node given at the version message. **/
        uint32_t nStartingHeight;


        /* Duplicates connection reset. */
        uint32_t nConsecutiveFails;


        /* Orphans connection reset. */
        uint32_t nConsecutiveOrphans;


        /** Flag to determine if a connection is Inbound. **/
        bool fInbound;


        /** Counter to keep track of the last time a ping was made. **/
        std::atomic<uint64_t> nLastPing;


        /** The last getblocks call this node has received. **/
        static memory::atomic<uint1024_t> hashLastGetblocks;


        /** The time since last getblocks call. **/
        static std::atomic<uint64_t> nLastGetBlocks;


        /** Handle an average calculation of fast sync blocks. */
        static std::atomic<uint64_t> nFastSyncAverage;


        /** The last time a block was accepted. **/
        static std::atomic<uint64_t> nLastTimeReceived;


        /** The trigger hash to send a continue inv message to remote node. **/
        uint1024_t hashContinue;


        /** timer object to keep track of ping latency. **/
        std::map<uint64_t, runtime::timer> mapLatencyTracker;


        /** Map to keep track of sent request ID's while witing for them to return. **/
        std::map<uint32_t, uint64_t> mapSentRequests;


        /** Event
         *
         *  Virtual Functions to Determine Behavior of Message LLP.
         *
         *  @param[in] EVENT The byte header of the event type.
         *  @param[in[ LENGTH The size of bytes read on packet read events.
         *
         */
        void Event(uint8_t EVENT, uint32_t LENGTH = 0) final;


        /** ProcessPacket
         *
         *  Main message handler once a packet is recieved.
         *
         *  @return True is no errors, false otherwise.
         *
         **/
        bool ProcessPacket() final;


        /** PushVersion
         *
         *  Handle for version message
         *
         **/
        void PushVersion();


        /** PushAddress
         *
         *  Send an LegacyAddress to Node.
         *
         *  @param[in] addr The address to send to nodes
         *
         **/
        void PushAddress(const LegacyAddress& addr);


        /** GetNode
         *
         *  Get a node by connected session.
         *
         *  @param[in] nSession The session to receive
         *
         *  @return a pointer to connected node.
         *
         **/
        static memory::atomic_ptr<LegacyNode>& GetNode(const uint64_t nSession);


        /** SessionActive
         *
         *  Determine whether a session is connected.
         *
         *  @param[in] nSession The session to check for
         *
         *  @return true if session is connected.
         *
         **/
        static bool SessionActive(const uint64_t nSession);


        /** DoS
         *
         *  Send the DoS Score to DDOS Filte
         *
         *  @param[in] nDoS The score to add for DoS banning
         *  @param[in] fReturn The value to return (False disconnects this node)
         *
         **/
        inline bool DoS(int nDoS, bool fReturn)
        {
            if(DDOS)
                DDOS->rSCORE += nDoS;

            return fReturn;
        }


        /** ReadPacket
         *
         *  Non-Blocking Packet reader to build a packet from TCP Connection.
         *  This keeps thread from spending too much time for each Connection.
         *
         **/
        void ReadPacket() final;


        /** PushGetBlocks
         *
         *  Send a request to get recent inventory from remote node.
         *
         *  @param[in] hashBlockFrom The block to start from
         *  @param[in] hashBlockTo The block to search to
         *
         **/
        void PushGetBlocks(const uint1024_t& hashBlockFrom, const uint1024_t& hashBlockTo);


        /** NewMessage
         *
         *  Creates a new message with a commands and data.
         *
         *  @param[in] chCommand Commands to add to the legacy packet.
         *  @param[in] ssData A datastream object with data to write.
         *
         *  @return Returns a filled out legacy packet.
         *
         **/
        static LegacyPacket NewMessage(const char* chCommand, const DataStream& ssData)
        {
            LegacyPacket RESPONSE(chCommand);
            RESPONSE.SetData(ssData);

            return RESPONSE;
        }

        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         *  @param[in] chCommand Commands to add to the legacy packet.
         *
         **/
        void PushMessage(const char* chCommand)
        {
            LegacyPacket RESPONSE(chCommand);
            RESPONSE.SetChecksum();

            this->WritePacket(RESPONSE);
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1>
        void PushMessage(const char* chMessage, const T1& t1)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4 << t5;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4 << t5 << t6;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4 << t5 << t6 << t7;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8;

            this->WritePacket(NewMessage(chMessage, ssData));
        }


        /** PushMessage
         *
         *  Adds a legacy packet to the queue to write to the socket.
         *
         **/
        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
        void PushMessage(const char* chMessage, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
        {
            DataStream ssData(SER_NETWORK, MIN_PROTO_VERSION);
            ssData << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9;

            this->WritePacket(NewMessage(chMessage, ssData));
        }

    };

}

#endif
