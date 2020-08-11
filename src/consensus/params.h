// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <uint256.h>
#include <limits>
#include <map>
#include <string>

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_CSV, // Deployment of BIP68, BIP112, and BIP113.
    DEPLOYMENT_SEGWIT, // Deployment of BIP141, BIP143, and BIP147.
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nTargetSpacing)
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    /** Proof of work parameters */
    uint256 powLimit;
    uint256 posLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;

    /** donu stuff */
    int64_t nTargetSpacing;
    int64_t nStakeMinAge;
    int64_t nStakeMaxAge;
    int64_t nModifierInterval;
    int nCoinbaseMaturity;  // Coinbase transaction outputs can only be spent after this number of new blocks (network rule)
    int nDGWForkHeight;

};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
