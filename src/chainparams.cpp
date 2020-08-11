// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <assert.h>
#include <memory>

#include <chainparamsseeds.h>

#include <arith_uint256.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTimeTx, uint32_t nTimeBlock, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(9999) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;
    txNew.nTime = nTimeTx;

    CBlock genesis;
    genesis.nTime    = nTimeBlock;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTimeTx, uint32_t nTimeBlock, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "1/17/2020 - NYTimes The $119 Billion Sea Wall That Could Defend New York … or Not";
    const CScript genesisOutputScript = CScript();
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTimeTx, nTimeBlock, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 20);
        consensus.posLimit = ArithToUint256(~arith_uint256(0) >> 32);
        
        consensus.nTargetSpacing = 2 * 60;  // 2 minute block spacing

        consensus.nStakeMinAge = 12 * 60 * 60; // 12 hours
        consensus.nStakeMaxAge = 30 * 24 * 60 * 60; // 30 days
        consensus.nModifierInterval = 30 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 100;

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nTargetSpacing

        // Hard fork to DGW diff algo
        consensus.nDGWForkHeight = 37271;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000025ad5ef67ac792e");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x000000036da48bb07243160aa6d7a6fe4629245dd1725292444ed8073c7f5a9b");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xd3;
        pchMessageStart[1] = 0x1a;
        pchMessageStart[2] = 0x3d;
        pchMessageStart[3] = 0xe4;
        vAlertPubKey = ParseHex("0450A906D1033FCFC53F2E622BBDA278271D6BF932FAFC36C7BDBD866C6396416283F5D348C3BCB2E6BB8EC80645678226C7B666C4F37BAFE3BA25EE1525AA5333");
        nDefaultPort = 15666;
        nPruneAfterHeight = 100000;
        nLastPOWBlock = 43200;


        genesis = CreateGenesisBlock(1579285001, 1579285478, 647857168, 0x1d00ffff, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000008507af1fdaaf3fed6173005b23b0febf72e7c2094f11f1d057692182"));
        assert(genesis.hashMerkleRoot == uint256S("0xea85a565112465377ff696122443ca31177d70efaafd3c760c45791b5c2649f5"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they dont support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.

        vSeeds.emplace_back("dnsseed.donu.io");
        vSeeds.emplace_back("nodes.donu.io");
        vSeeds.emplace_back("dnsseed.donu.network");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,53);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,177);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        // human readable prefix to bench32 address
        bech32_hrp = "dn";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {     0, uint256S("0x000000008507af1fdaaf3fed6173005b23b0febf72e7c2094f11f1d057692182")},
                {     3, uint256S("0x00000059a98da88692ee7222baa16eb2dacfae28a13ff81e01486fd8c1b93c88")},
                {     5, uint256S("0x000000637244d847e561a4f750e4566e06f7124c8bcdf69c64924c23fdd3400c")},
                {   111, uint256S("0x00000001e62baa68912d45062ac085d1f4c6e515c2b649fd2ba9447fb306c22f")},
                {   142, uint256S("0x0000000299b167cfff1be134fc1c60df999f17740113f9f5b128737ca10fa3e0")},
                {   173, uint256S("0x00000001e1f456d9cbdee85b3002791baec33329c81845e1a9d756c030a60325")},
                { 24610, uint256S("0x00000000000003244a808885393f8b8fab258b8e4b15d9bf3b55e2485e44fdde")},
                { 43201, uint256S("0xcf6639b07a72ff6e41d398c9c20becf624ce379afc209980ab262e1c8c300952")},
                { 43788, uint256S("0x4663e014eff97e59ff801d0d2b79c36816aa3f30e834816d8e779b96e2313fad")},
            }
        };

        chainTxData = ChainTxData{
            1596913697, // * UNIX timestamp of last known number of transactions
            80183,      // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.00347     // * estimated number of transactions per second after that timestamp
                        // 1561895/(1544091904-1345400356) = 0.00786
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 20);
        consensus.posLimit = ArithToUint256(~arith_uint256(0) >> 20);

        consensus.nTargetSpacing = 2 * 60; // 2 minute block spacing

        consensus.nStakeMinAge = 1 * 60 * 60; // 1 hour
        consensus.nStakeMaxAge = 30 * 24 * 60 * 60; // 30 days
        consensus.nModifierInterval = 30 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 50;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nTargetSpacing

        // Hard fork to DGW diff algo
        consensus.nDGWForkHeight = 1840;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        pchMessageStart[0] = 0xab;
        pchMessageStart[1] = 0xf0;
        pchMessageStart[2] = 0xb8;
        pchMessageStart[3] = 0xed;
        vAlertPubKey = ParseHex("04383862439513e940f6fcbf62d365c162a5256920c2c25b0b4266fdee4a443d71cfe224dbccff6fdb2ea57a37eb0cbec5637ebea06f63c70ca093672fbdc27643");
        nDefaultPort = 15668;
        nPruneAfterHeight = 1000;
        nLastPOWBlock = 999999;

        genesis = CreateGenesisBlock(1592011840, 1592011846, 44822, 0x1e0ffff0, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000d7eefc6c063fc8c708bcb07b6dac874cbb979baa075b95b954e602e52d4"));
        assert(genesis.hashMerkleRoot == uint256S("0xcd533d6bd27145274e19885d19cfe8121aee26761bf2916ad3d908f8d17303fb"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("dnsseed.donu.io");
        vSeeds.emplace_back("nodes.donu.io");
        vSeeds.emplace_back("dnsseed.donu.network");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,8);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,125);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // human readable prefix to bench32 address
        bech32_hrp = "tdn";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;


        checkpointData = {
            {
                {     0, uint256S("0x00000d7eefc6c063fc8c708bcb07b6dac874cbb979baa075b95b954e602e52d4")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 000000000000033cfa3c975eb83ecf2bb4aaedf68e6d279f6ed2b427c64caff9 (height 1260526)
            1592011846,
            0,
            0
        };

    }
};

/**
 * Regression test
 */

class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 10);
        consensus.posLimit = ArithToUint256(~arith_uint256(0) >> 10);

        consensus.nTargetSpacing = 2 * 60;  // 2 minute block spacing 

        consensus.nStakeMinAge = 1 * 60 * 60; // 1 hour
        consensus.nStakeMaxAge = 30 * 24 * 60 * 60; // 30 days
        consensus.nModifierInterval = 30 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 50;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        
        // Hard fork to DGW diff algo
        consensus.nDGWForkHeight = 1840;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        pchMessageStart[0] = 0xb8;
        pchMessageStart[1] = 0xcb;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xfe;
        vAlertPubKey = ParseHex("04383862439513e940f6fcbf62d365c162a5256920c2c25b0b4266fdee4a443d71cfe224dbccff6fdb2ea57a37eb0cbec5637ebea06f63c70ca093672fbdc27643");
        nDefaultPort = 15670;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1592071198, 1592071200, 4416963, 0x1e0ffff0, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000006f1b2bcc44235d0046693c03b4a998955b9f6e9b0caa8173e45b5dc63dd"));
        assert(genesis.hashMerkleRoot == uint256S("0xbdc7b3af9271e4d3a441e237d38a89ccee8a8cfd8b1f64f882ef483cd633a57a"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        checkpointData = {
            {
                {0, uint256S("0x000006f1b2bcc44235d0046693c03b4a998955b9f6e9b0caa8173e45b5dc63dd")},
            }
        };

        chainTxData = ChainTxData{
            1592071200,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,122);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,15);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,125);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "dnrt";

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}
