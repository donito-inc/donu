// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

#include <bignum.h>
#include <chainparams.h>

#include <math.h>

// donu: find last block index up to pindex
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake)
{
    while (pindex && pindex->pprev && (pindex->IsProofOfStake() != fProofOfStake))
        pindex = pindex->pprev;
    return pindex;
}

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake, const Consensus::Params& params)
{
    if (Params().NetworkIDString() == CBaseChainParams::TESTNET) {
        if (pindexLast->nHeight >= 1840)
            return DarkGravityWave(pindexLast, fProofOfStake, params);
        else
            return DualKGW3(pindexLast, fProofOfStake, params);
    }

    if (pindexLast->nHeight >= 37271)
        return DarkGravityWave(pindexLast, fProofOfStake, params);
    else
        return DualKGW3(pindexLast, fProofOfStake, params);
}

unsigned int DualKGW3(const CBlockIndex* pindexLast, bool fProofOfStake, const Consensus::Params& params)
{
    const CBlockIndex* BlockLastSolved = GetLastBlockIndex(pindexLast, fProofOfStake);
    const CBlockIndex* BlockReading = GetLastBlockIndex(pindexLast, fProofOfStake);
    int64_t PastBlocksMass = 0;
    int64_t PastRateActualSeconds = 0;
    int64_t PastRateTargetSeconds = 0;
    double PastRateAdjustmentRatio = double(1);
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;
    double EventHorizonDeviation;
    double EventHorizonDeviationFast;
    double EventHorizonDeviationSlow;
    static const int64_t Blocktime = params.nTargetSpacing;
    static const unsigned int timeDaySeconds = 86400;
    uint64_t pastSecondsMin = timeDaySeconds * 0.025;
    uint64_t pastSecondsMax = timeDaySeconds * 7;
    uint64_t PastBlocksMin = pastSecondsMin / Blocktime;
    uint64_t PastBlocksMax = pastSecondsMax / Blocktime;
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    const arith_uint256 bnPosLimit = UintToArith256(params.posLimit);

    if (BlockLastSolved == nullptr || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) {
        if (!fProofOfStake)
            return bnPowLimit.GetCompact();
        else
            return bnPosLimit.GetCompact();
    }

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) break;
        PastBlocksMass++;
        PastDifficultyAverage.SetCompact(BlockReading->nBits);
        if (i > 1) {
            if (PastDifficultyAverage >= PastDifficultyAveragePrev)
                PastDifficultyAverage = ((PastDifficultyAverage - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev;
            else
                PastDifficultyAverage = PastDifficultyAveragePrev - ((PastDifficultyAveragePrev - PastDifficultyAverage) / i);
        }
        PastDifficultyAveragePrev = PastDifficultyAverage;
        PastRateActualSeconds = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
        PastRateTargetSeconds = Blocktime * PastBlocksMass;
        PastRateAdjustmentRatio = double(1);
        if (PastRateActualSeconds < 0) PastRateActualSeconds = 0;
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0)
            PastRateAdjustmentRatio = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
        EventHorizonDeviation = 1 + (0.7084 * pow((double(PastBlocksMass) / double(72)), -1.228));
        EventHorizonDeviationFast = EventHorizonDeviation;
        EventHorizonDeviationSlow = 1 / EventHorizonDeviation;

        if (PastBlocksMass >= PastBlocksMin) {
            if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) {
                assert(BlockReading);
                break;
            }
        }
        if (BlockReading->pprev == nullptr) {
            assert(BlockReading);
            break;
        }
        BlockReading = BlockReading->pprev;
    }

    arith_uint256 kgw_dual1(PastDifficultyAverage);
    arith_uint256 kgw_dual2;
    kgw_dual2.SetCompact(pindexLast->nBits);
    if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
        kgw_dual1 *= PastRateActualSeconds;
        kgw_dual1 /= PastRateTargetSeconds;
    }
    int64_t nActualTime1 = pindexLast->GetBlockTime() - pindexLast->pprev->GetBlockTime();
    int64_t nActualTimespanshort = nActualTime1;

    if (nActualTime1 < 0) nActualTime1 = Blocktime;
    if (nActualTime1 < Blocktime / 3) nActualTime1 = Blocktime / 3;
    if (nActualTime1 > Blocktime * 3) nActualTime1 = Blocktime * 3;
    kgw_dual2 *= nActualTime1;
    kgw_dual2 /= Blocktime;
    arith_uint256 bnNew;
    bnNew = ((kgw_dual2 + kgw_dual1) / 2);

    if (nActualTimespanshort < Blocktime / 6) {
        const int nLongShortNew1 = 85;
        const int nLongShortNew2 = 100;
        bnNew = bnNew * nLongShortNew1;
        bnNew = bnNew / nLongShortNew2;
    }

    if (!fProofOfStake && bnNew > bnPowLimit)
        bnNew = bnPowLimit;
    if (fProofOfStake && bnNew > bnPosLimit)
        bnNew = bnPosLimit;

    return bnNew.GetCompact();
}

unsigned int DarkGravityWave(const CBlockIndex* pindexLast, bool fProofOfStake, const Consensus::Params& params) {
    /* DarkGravity v3, written by Evan Duffield - evan@dash.org */
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    const arith_uint256 bnPosLimit = UintToArith256(params.posLimit);
    const CBlockIndex *pindex = pindexLast;
    const CBlockIndex* BlockLastSolved = GetLastBlockIndex(pindexLast, fProofOfStake);
    int64_t nPastBlocks = 24;
    arith_uint256 bnPastTargetAvg;

    // make sure we have at least (nPastBlocks + 1) blocks, otherwise just return powLimit/posLimit
    if (BlockLastSolved == nullptr || BlockLastSolved->nHeight < nPastBlocks) {
        if (!fProofOfStake)
            return bnPowLimit.GetCompact();
        else
            return bnPosLimit.GetCompact();
    }

    // Reset PoS difficulty
    if (Params().NetworkIDString() == CBaseChainParams::TESTNET) {
        if (pindex->nHeight >= 1840 && pindex->nHeight <= 1940) {
            if (fProofOfStake) return bnPosLimit.GetCompact();
        }
    }
    if (pindex->nHeight >= 37271 && pindex->nHeight <= 37421) {
        if (fProofOfStake) return bnPosLimit.GetCompact();
    }

    for (unsigned int nCountBlocks = 1; nCountBlocks <= nPastBlocks;) {
        if (!pindex) {
            if (!fProofOfStake)
                return bnPowLimit.GetCompact();
            else
                return bnPosLimit.GetCompact();
        }

        // Average for POW or POS
        if (pindex->IsProofOfStake() != fProofOfStake) {
            pindex = pindex->pprev;
            continue;
        }

        nCountBlocks++;
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if (nCountBlocks != nPastBlocks) {
            assert(pindex->pprev); // should never fail
            pindex = pindex->pprev;
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    // NOTE: is this accurate? nActualTimespan counts it for (nPastBlocks - 1) blocks only...
    int64_t nTargetTimespan = nPastBlocks * params.nTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (!fProofOfStake && bnNew > bnPowLimit)
        bnNew = bnPowLimit;
    if (fProofOfStake && bnNew > bnPosLimit)
        bnNew = bnPosLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
