/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#include <TAO/Register/include/create.h>

#include <TAO/Register/include/enum.h>

/* Global TAO namespace. */
namespace TAO
{

    /* Register Layer namespace. */
    namespace Register
    {

        /* Generate a new account object register. */
        Object CreateAccount(const uint256_t& hashIdentifier)
        {
            /* Create an account object register. */
            TAO::Register::Object account;

            /* Generate the object register values. */
            account << std::string("balance")       << uint8_t(TYPES::MUTABLE)   << uint8_t(TYPES::UINT64_T) << uint64_t(0)
                    << std::string("token")         << uint8_t(TYPES::UINT256_T) << hashIdentifier;

            return account;
        }


        /* Generate a new trust object register. */
        Object CreateTrust()
        {
            /* Create an account object register. */
            TAO::Register::Object trust;

            /* Generate the object register values. */
            trust   << std::string("balance")       << uint8_t(TYPES::MUTABLE)  << uint8_t(TYPES::UINT64_T) << uint64_t(0)
                    << std::string("trust")         << uint8_t(TYPES::MUTABLE)  << uint8_t(TYPES::UINT64_T) << uint64_t(0)
                    << std::string("stake")         << uint8_t(TYPES::MUTABLE)  << uint8_t(TYPES::UINT64_T) << uint64_t(0)
                    << std::string("token")         << uint8_t(TYPES::UINT256_T) << uint256_t(0);

            return trust;
        }


        /* Generate a new token object register. */
        Object CreateToken(const uint256_t& hashIdentifier, const uint64_t nSupply, const uint8_t nDecimals)
        {
            /* Create an token object register. */
            TAO::Register::Object token;

            /* Generate the object register values. */
            token   << std::string("balance")       << uint8_t(TYPES::MUTABLE)  << uint8_t(TYPES::UINT64_T) << nSupply
                    << std::string("token")         << uint8_t(TYPES::UINT256_T) << hashIdentifier
                    << std::string("supply")        << uint8_t(TYPES::UINT64_T) << nSupply
                    << std::string("decimals")        << uint8_t(TYPES::UINT8_T) << nDecimals;

            return token;
        }


        /* Generate a new asset object register. */
        Object CreateAsset()
        {
            /* Create an asset object register. */
            TAO::Register::Object asset;

            return asset;
        }


        /* Generate a new namespace object register. */
        Object CreateNamespace(const std::string& strName)
        {
            /* Create a name object register. */
            TAO::Register::Object name;

            /* Generate the new name register with the address value. */
            name    << std::string("namespace")          << uint8_t(TYPES::STRING)  << strName;

            return name;
        }


        /* Generate a new name object register. */
        Object CreateName(const std::string& strNamespace, const std::string& strName, const uint256_t& hashRegister)
        {
            /* Create a name object register. */
            TAO::Register::Object name;

            /* Generate the new name register with the address value. */
            name    << std::string("namespace")     << uint8_t(TYPES::STRING)  << strNamespace
                    << std::string("name")          << uint8_t(TYPES::STRING)  << strName
                    << std::string("address")       << uint8_t(TYPES::MUTABLE) << uint8_t(TYPES::UINT256_T) << hashRegister;

            return name;
        }

    }
}
