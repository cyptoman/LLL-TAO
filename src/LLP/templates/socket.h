/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2018] ++

            (c) Copyright The Nexus Developers 2014 - 2018

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#ifndef NEXUS_LLP_TEMPLATES_SOCKET_H
#define NEXUS_LLP_TEMPLATES_SOCKET_H

#include <string>
#include <vector>
#include <stdio.h>

namespace LLP
{
    class CService;
    class CAddress;

    /* Base Template class to handle outgoing / incoming LLP data for both Client and Server. */
    class Socket
    {
    protected:

        /** The socket file identifier. **/
        int nSocket;

    public:

        /** The address of this connection. */
        CAddress addr;


        /** The default constructor. **/
        Socket() : nSocket() {}


        /** The socket constructor. **/
        Socket(int nSocketIn, CAddress addrIn) : nSocket(nSocketIn), addr(addrIn) {}


        /** Constructor for Address
         *
         *  @param[in] addrConnect The address to connect socket to
         *
         **/
        Socket(CService addrDest);


        /** IsValid
         *
         *  Checks if the socket is in a valid state.
         *
         *  @return true if the socket is in a valid state.
         *
         **/
        bool IsValid();


        /** Connect
         *
         *  Connects the socket to an external address
         *
         *  @param[in] addrConnect The address to connect to
         *
         *  @return true if the socket is in a valid state.
         *
         **/
        bool Connect(CService addrDest, int nTimeout = 5000);


        /** Available
         *
         *  Poll the socket to check for available data
         *
         *  @return the total bytes available for read
         *
         **/
        int Available();


        /** Disconnect
         *
         *  Clear resources associated with socket and return to invalid state.
         *
         **/
        void Disconnect();


        /** Read
         *
         *  Read data from the socket buffer non-blocking
         *
         *  @param[out] vData The byte vector to read into
         *  @param[in] nBytes The total bytes to read
         *
         *  @return the total bytes that were read
         *
         **/
        int Read(std::vector<uint8_t> &vData, size_t nBytes);


        /** Write
         *
         *  Write data into the socket buffer non-blocking
         *
         *  @param[in] vData The byte vector of data to be written
         *  @param[in] nBytes The total bytes to write
         *
         *  @return the total bytes that were written
         *
         **/
        int Write(std::vector<uint8_t> vData, size_t nBytes);

    };
}

#endif
