#ifndef CORE_RENSA_TRACKER_H_
#define CORE_RENSA_TRACKER_H_

#include <glog/logging.h>

#include "core/rensa_result.h"

// RensaTracker tracks how rensa is vanished.
// For example, a puyo is vanished in what-th chain, coefficient of each chain, etc.
// You can pass a RensaTracker to CoreField::simulate() to track the rensa.
// You can also define you own RensaTracker, and pass it to CoreField::simulate().
//
// RensaTracker must define several interface. CoreField::simulate() has several hook poinits
// that calls the corresponding Tracker methods. If you'd like to add a new hook point,
// you need to define a hook point in CoreField.

// RensaNonTracker is a tracker that does not track anything.
class RensaNonTracker {
public:
    void colorPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) { }
    void ojamaPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) { }
    void puyoIsDropped(int /*x*/, int /*fromY*/, int /*toY*/) { }
    void nthChainDone(int /*nthChain*/, int /*numErasedPuyo*/, int /*coef*/) {}
};

template<typename TrackResult>
class RensaTracker;

// RensaTracker<RensaChainTrackResult> tracks in what-th rensa a puyo is vanished.
template<>
class RensaTracker<RensaChainTrackResult> {
public:
    RensaTracker() :
        originalY_ {
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
        }
    {
    }

    const RensaChainTrackResult& result() const { return result_; }

    void colorPuyoIsVanished(int x, int y, int nthChain) { result_.setErasedAt(x, originalY_[x][y], nthChain); }
    void ojamaPuyoIsVanished(int x, int y, int nthChain) { result_.setErasedAt(x, originalY_[x][y], nthChain); }
    void puyoIsDropped(int x, int fromY, int toY) { originalY_[x][toY] = originalY_[x][fromY]; }
    void nthChainDone(int /*nthChain*/, int /*numErasedPuyo*/, int /*coef*/) {}

private:
    int originalY_[FieldConstant::MAP_WIDTH][FieldConstant::MAP_HEIGHT];
    RensaChainTrackResult result_;
};
typedef RensaTracker<RensaChainTrackResult> RensaChainTracker;

template<>
class RensaTracker<RensaCoefResult> {
public:
    const RensaCoefResult& result() const { return result_; }

    void colorPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) {}
    void ojamaPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) {}
    void puyoIsDropped(int /*x*/, int /*fromY*/, int /*toY*/) {}
    void nthChainDone(int nthChain, int numErasedPuyo, int coef) { result_.setCoef(nthChain, numErasedPuyo, coef); }

private:
    RensaCoefResult result_;
};
typedef RensaTracker<RensaCoefResult> RensaCoefTracker;

template<>
class RensaTracker<RensaVanishingPositionResult> {
public:
    RensaTracker()
    {
        resetY();
    }

    const RensaVanishingPositionResult& result() const { return result_; }

    void colorPuyoIsVanished(int x, int y, int nthChain)
    {
        if (yAtPrevRensa_[x][y] == 0) {
            result_.setBasePuyo(x, y, nthChain);
        } else {
            result_.setFallingPuyo(x, yAtPrevRensa_[x][y], y, nthChain);
        }
    }

    void ojamaPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) {}
    void puyoIsDropped(int x, int fromY, int toY) { yAtPrevRensa_[x][toY] = fromY; }
    void nthChainDone(int /*nthChain*/, int /*numErasedPuyo*/, int /*coef*/) { resetY(); }

private:
    void resetY() {
        constexpr std::array<int, FieldConstant::MAP_HEIGHT> ALL_ZERO {{}};
        yAtPrevRensa_.fill(ALL_ZERO);
    }

    RensaVanishingPositionResult result_;
    std::array<std::array<int, FieldConstant::MAP_HEIGHT>, FieldConstant::MAP_WIDTH> yAtPrevRensa_;
};
typedef RensaTracker<RensaVanishingPositionResult> RensaVanishingPositionTracker;

// ----------------------------------------------------------------------

class RensaChainPointerTracker {
public:
    explicit RensaChainPointerTracker(RensaChainTrackResult* trackResult) :
        originalY_ {
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, },
        },
        result_(trackResult)
    {
        // TODO(mayah): Assert trackResult is initialized?
    }

    const RensaChainTrackResult& result() const { return *result_; }

    void colorPuyoIsVanished(int x, int y, int nthChain) { result_->setErasedAt(x, originalY_[x][y], nthChain); }
    void ojamaPuyoIsVanished(int x, int y, int nthChain) { result_->setErasedAt(x, originalY_[x][y], nthChain); }
    void puyoIsDropped(int x, int fromY, int toY) { originalY_[x][toY] = originalY_[x][fromY]; }
    void nthChainDone(int /*nthChain*/, int /*numErasedPuyo*/, int /*coef*/) {}

private:
    int originalY_[FieldConstant::MAP_WIDTH][FieldConstant::MAP_HEIGHT];
    RensaChainTrackResult* result_;
};

class RensaYPositionTracker {
public:
    RensaYPositionTracker() :
        originalY_ {
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        }
    {
    }

    int originalY(int x, int y) const { return originalY_[x][y]; }

    void colorPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) {}
    void ojamaPuyoIsVanished(int /*x*/, int /*y*/, int /*nthChain*/) {}
    void puyoIsDropped(int x, int fromY, int toY)
    {
        DCHECK_NE(fromY, toY);
        originalY_[x][toY] = originalY_[x][fromY];
        originalY_[x][fromY] = 0;
    }
    void nthChainDone(int /*nthChain*/, int /*numErasedPuyo*/, int /*coef*/) {}

private:
    int originalY_[FieldConstant::MAP_WIDTH][FieldConstant::MAP_HEIGHT];
};

#endif