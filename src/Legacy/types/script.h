/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2018

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#ifndef NEXUS_TAO_LEGACY_TYPES_SCRIPT_H
#define NEXUS_TAO_LEGACY_TYPES_SCRIPT_H

#include <Legacy/types/enum.h>
#include <Legacy/types/address.h>

#include <LLC/types/bignum.h>
#include <LLC/include/key.h>
#include <Util/include/base58.h>

#include <string>
#include <vector>
#include <cassert>

namespace Legacy
{
    class Transaction;

    /** Value String
     *
     *  Returns a string in integer value.
     *
     *  @param[in] vch the byte sequence to convert.
     *
     *  @return The return string with value output
     *
     **/
    std::string ValueString(const std::vector<uint8_t>& vch);


    /** Stack String
     *
     *  Builds a string of values in a stack seperated by spaces
     *
     *  @param[in] vStack The stack of data to output.
     *
     *  @return the return string with stack output.
     *
     **/
    std::string StackString(const std::vector<std::vector<uint8_t> >& vStack);



    /** Serialized script, used inside transaction inputs and outputs */
    class CScript : public std::vector<uint8_t>
    {
    protected:

        /** push_int64
         *
         *  Push a 64 bit signed int onto the stack.
         *
         *  @param[in] n The Integer to push into the script.
         *
         **/
        CScript& push_int64(int64_t n);

        /** push_uint64
         *
         *  Push a 64 bit unsigned int onto the stack.
         *
         *  @param[in] n The Integer to push into the script.
         *
         **/
        CScript& push_uint64(uint64_t n);

    public:

        /** Default Constructors **/
        CScript() { }


        /** Construct from another CScript
         *
         *  Initialize with a copy of the CScript
         *
         *  @param[in] b The object to copy into the vector.
         *
         **/
        CScript(const CScript& b) : std::vector<uint8_t>(b.begin(), b.end()) { }


        /** Construct from iterators
         *
         *  Initialize with a copy using iterators
         *
         *  @param[in] pbegin The begin iterator of object
         *  @param[in] pend The end iterator of object
         *
         **/
        CScript(const_iterator pbegin, const_iterator pend) : std::vector<uint8_t>(pbegin, pend) { }


    #ifndef _MSC_VER

        /** Construct from iterators
         *
         *  Initialize with a copy using iterators
         *
         *  @param[in] pbegin The begin iterator of object
         *  @param[in] pend The end iterator of object
         *
         **/
        CScript(const uint8_t* pbegin, const uint8_t* pend) : std::vector<uint8_t>(pbegin, pend) { }
    #endif


        /** GetOP
         *
         *  Get the op codes from stack
         *
         *  @param[in] pc Iterator to begin
         *  @param[out] opcodeRet The code to be returned
         *  @param[out] vchRet The bytes to return
         *
         *  @return true if successful, false otherwise
         *
         **/
        bool GetOp(iterator& pc, opcodetype& opcodeRet, std::vector<uint8_t>& vchRet);


        /** GetOP
         *
         *  Get the op codes from stack
         *
         *  @param[in] pc Iterator to begin
         *  @param[out] opcodeRet The code to be returned
         *
         *  @return true if successful, false otherwise
         *
         **/
        bool GetOp(iterator& pc, opcodetype& opcodeRet);


        /** GetOP
         *
         *  Get the op codes from stack
         *
         *  @param[in] pc constant iterator to begin
         *  @param[out] opcodeRet The code to be returned
         *  @param[out] vchRet The bytes to return
         *
         *  @return true if successful, false otherwise
         *
         **/
        bool GetOp(const_iterator& pc, opcodetype& opcodeRet, std::vector<uint8_t>& vchRet) const;


        /** GetOP
         *
         *  Get the op codes from stack
         *
         *  @param[in] pc constant iterator to begin
         *  @param[out] opcodeRet The code to be returned
         *
         *  @return true if successful, false otherwise
         *
         **/
        bool GetOp(const_iterator& pc, opcodetype& opcodeRet) const;


        /** GetOP
         *
         *  Get the op codes from stack
         *
         *  @param[in] pc constant iterator to begin
         *  @param[out] opcodeRet The code to be returned
         *  @param[out] pvchRet The bytes to return via pointer
         *
         *  @return true if successful, false otherwise
         *
         **/
        bool GetOp2(const_iterator& pc, opcodetype& opcodeRet, std::vector<uint8_t>* pvchRet) const;


        /** DecodeOP_N
         *
         *  Decodes the operation code.
         *
         *  @param[in] opcode The code to decode
         *
         *  @return the int representing code.
         *
         **/
        int32_t DecodeOP_N(opcodetype opcode) const;


        /** EncodeOP_N
         *
         *  Encodes the operation code.
         *
         *  @param[in] n The int to encode with
         *
         *  @return the opcode corresponding to n
         *
         **/
        opcodetype EncodeOP_N(int32_t n);


        /** FindAndDelete
         *
         *  Removes a script from a script
         *
         *  @param[in] b The script to delete from this script
         *
         *  @return the place in which was overwritten.
         *
         **/
        int32_t FindAndDelete(const CScript& b);


        /** Find
         *
         *  Find the location of an opcode in the script.
         *
         *  @param[in] op The code that is being searched for
         *
         *  @return the byte location where code was found.
         *
         **/
        int32_t Find(opcodetype op) const;


        /** GetSigOpCount
         *
         *  Get the total number of signature operations
         *
         *  @param[in] fAccurate Accuracy flag for SigOps
         *
         *  @return the total number of signature operations
         *
         **/
        uint32_t GetSigOpCount(bool fAccurate) const;


        /** GetSigOpCount
         *
         *  Get the total number of signature operations
         *
         *  @param[in] scriptSig The script object to check
         *
         *  @return the total number of signature operations
         *
         **/
        uint32_t GetSigOpCount(const CScript& scriptSig) const;


        /** IsPayToScriptHash
         *
         *  Determine if script fits P2SH template
         *
         *  @return True if it fits P2SH template
         *
         **/
        bool IsPayToScriptHash() const;


        /** IsPushOnly
         *
         *  Determine if script is a pushdata only script
         *
         *  @return True if it is only pushing data
         *
         **/
        bool IsPushOnly() const;


        /** SetNexusAddress
         *
         *  Set the nexus address into script
         *
         *  @param[in] address The nexus address you input
         *
         **/
        void SetNexusAddress(const NexusAddress& address);


        /** SetNexusAddress
         *
         *  Set the nexus address from public key
         *
         *  @param[in] vchPubKey The public key to set.
         *
         **/
        void SetNexusAddress(const std::vector<uint8_t>& vchPubKey);


        /** SetMultiSig
         *
         *  Set script based on multi-sig data
         *
         *  @param[in] nRequired The total multi-sig keys required
         *  @param[in] keys The keys to be added into the multi-sig contract
         *
         **/
        void SetMultisig(int nRequired, const std::vector<LLC::ECKey>& keys);


        /** SetPayToScriptHash
         *
         *  Set the script based on a P2SH script input
         *
         *  @param[in] subscript The input script
         *
         **/
        void SetPayToScriptHash(const CScript& subscript);


        /** PrintHex
         *
         *  Print the Hex output of the script
         *
         **/
        void PrintHex() const;


        /** ToString
         *
         *  Print the Hex output of the script into a std::string
         *
         *  @param[in] fShort Flag to set string to shorthand form
         *
         *  @return The script hex output in std::string
         *
         **/
        std::string ToString(bool fShort=false) const;


        /** print
         *
         *  Dump the Hex data into std::out or console
         *
         **/
        void print() const;


        /** Operator overload +=
         *
         *  Concatenate Script Objects
         *
         **/
        CScript& operator+=(const CScript& b)
        {
            insert(end(), b.begin(), b.end());
            return *this;
        }


        /** Operator overload +
         *
         *  Concatenate two Script Objects
         *
         **/
        friend CScript operator+(const CScript& a, const CScript& b)
        {
            CScript ret = a;
            ret += b;
            return ret;
        }


        //explicit CScript(char b) is not portable.  Use 'signed char' or 'uint8_t'.
        explicit CScript(int8_t b)                      { operator<<(b); }
        explicit CScript(int16_t b)                     { operator<<(b); }
        explicit CScript(int32_t b)                     { operator<<(b); }
        explicit CScript(int64_t b)                     { operator<<(b); }
        explicit CScript(uint8_t b)                     { operator<<(b); }
        explicit CScript(uint16_t b)                    { operator<<(b); }
        explicit CScript(uint32_t b)                    { operator<<(b); }
        explicit CScript(uint64_t b)                    { operator<<(b); }
        explicit CScript(opcodetype b)                  { operator<<(b); }
        explicit CScript(const uint256_t& b)            { operator<<(b); }
        explicit CScript(const LLC::CBigNum& b)         { operator<<(b); }
        explicit CScript(const std::vector<uint8_t>& b) { operator<<(b); }

        CScript& operator<<(int8_t b)                   { return push_int64(b); }
        CScript& operator<<(int16_t b)                  { return push_int64(b); }
        CScript& operator<<(int32_t b)                  { return push_int64(b); }
        CScript& operator<<(int64_t b)                  { return push_int64(b); }
        CScript& operator<<(uint8_t b)                  { return push_uint64(b); }
        CScript& operator<<(uint16_t b)                 { return push_uint64(b); }
        CScript& operator<<(uint32_t b)                 { return push_uint64(b); }
        CScript& operator<<(uint64_t b)                 { return push_uint64(b); }

        CScript& operator<<(opcodetype opcode)
        {
            if (opcode < 0 || opcode > 0xff)
                throw std::runtime_error("CScript::operator<<() : invalid opcode");
            insert(end(), (uint8_t)opcode);
            return *this;
        }

        CScript& operator<<(const uint256_t& b)
        {
            insert(end(), sizeof(b));
            insert(end(), (uint8_t*)&b, (uint8_t*)&b + sizeof(b));
            return *this;
        }

        CScript& operator<<(const LLC::CBigNum& b)
        {
            *this << b.getvch();
            return *this;
        }

        CScript& operator<<(const std::vector<uint8_t>& b)
        {
            if (b.size() < OP_PUSHDATA1)
            {
                insert(end(), (uint8_t)b.size());
            }
            else if (b.size() <= 0xff)
            {
                insert(end(), OP_PUSHDATA1);
                insert(end(), (uint8_t)b.size());
            }
            else if (b.size() <= 0xffff)
            {
                insert(end(), OP_PUSHDATA2);
                uint16_t nSize = b.size();
                insert(end(), (uint8_t*)&nSize, (uint8_t*)&nSize + sizeof(nSize));
            }
            else
            {
                insert(end(), OP_PUSHDATA4);
                uint32_t nSize = b.size();
                insert(end(), (uint8_t*)&nSize, (uint8_t*)&nSize + sizeof(nSize));
            }
            insert(end(), b.begin(), b.end());
            return *this;
        }

        CScript& operator<<(const CScript& b)
        {
            // I'm not sure if this should push the script or concatenate scripts.
            // If there's ever a use for pushing a script onto a script, delete this member fn
            assert(!"warning: pushing a CScript onto a CScript with << is probably not intended, use + to concatenate");
            return *this;
        }
    };
}

#endif