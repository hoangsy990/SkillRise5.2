#pragma once

// Candidate-only class contract for the S21 Grow Lancer source class.
//
// The SS6 client/server currently expose seven class slots.  This header does
// not widen those arrays or activate class 7.  It provides the exact wire
// algebra and a fail-closed gate so a future native class integration cannot
// silently reuse Dark Lord or truncate a DB class value.
namespace rise { namespace growlancer {

constexpr unsigned kGrowLancerSourceClass = 7;
constexpr unsigned kGrowLancerDbClassBase = 112; // 7 * 16 + evolution 0
constexpr unsigned kGrowLancerDbClassLast = 115; // evolution 3 lead

constexpr bool IsGrowLancerDbClass(unsigned dbClass)
{
    return dbClass >= kGrowLancerDbClassBase && dbClass <= kGrowLancerDbClassLast;
}

// Native DS/GS class wire conversion used by the existing SS6 protocol.
// This is algebra only; it is not evidence that DB 112..115 is accepted by
// the current character-create or persistence handlers.
constexpr unsigned GrowLancerDbToWire(unsigned dbClass)
{
    const unsigned evolution = dbClass % 16;
    const unsigned base = dbClass / 16;
    unsigned wire = evolution * 16;
    wire -= wire / 32;
    wire += base * 32;
    return wire;
}

constexpr bool IsGrowLancerWireCandidate(unsigned dbClass, unsigned wireClass)
{
    // The legacy DS/GS field is BYTE-sized.  DB115's algebraic value 271 is
    // intentionally rejected until a verified wider packet contract exists.
    return IsGrowLancerDbClass(dbClass) &&
        wireClass <= 0xffu &&
        GrowLancerDbToWire(dbClass) == wireClass;
}

struct GrowLancerClassActivationContext
{
    bool nativeClassSlot;
    bool defaultRows;
    bool createAndLogin;
    bool packetRoundTrip;
    bool persistence;
    bool classConsumers;
};

// No activation is permitted until every cross-cutting native contract is
// proven.  The current SS6 call site should pass all false and continue to
// reject Grow Lancer learning/casting.
constexpr bool CanActivateGrowLancerClass(
    const GrowLancerClassActivationContext& context)
{
    return context.nativeClassSlot && context.defaultRows &&
        context.createAndLogin && context.packetRoundTrip &&
        context.persistence && context.classConsumers;
}

} }
