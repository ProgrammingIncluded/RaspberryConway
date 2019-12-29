/***********************************************
 * Project: RaspberryConway
 * File: game.hpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
 * Inspired by: https://dotat.at/prog/life/hashlife.c
***********************************************/
#ifndef GAME_HPP
#define GAME_HPP

#include <cstring>
#include <unordered_map>

struct GenData {
    char data;
    GenData *ne;
    GenData *nw;
    GenData *sw;
    GenData *se;
    GenData *nn;
    GenData *ee;
    GenData *ww;
    GenData *ss;
    GenData *cc;

    bool operator==(const GenData &g) const {
        return 
            data == g.data &&
            ne == g.ne &&
            nw == g.nw &&
            sw == g.sw &&
            se == g.se &&
            nn == g.nn &&
            ee == g.ee &&
            ww == g.ww &&
            ss == g.ss &&
            cc == g.cc;
    }
};

#define se(i) ((bool)(i->data & 0b00000001))
#define sw(i) ((bool)(i->data & 0b00000010))
#define ne(i) ((bool)(i->data & 0b00000100))
#define nw(i) ((bool)(i->data & 0b00001000))

struct hash_fun {
    std::size_t operator() (const GenData &d) const {
        auto hasher = std::hash<GenData*>();
        // No need to hash data since that is done by basis
        return hasher(d.ne) ^ hasher(d.nw) ^ hasher(d.sw) ^ hasher(d.se) ^ hasher(d.nn) ^ hasher(d.ee)
                ^ hasher(d.ww) ^ hasher(d.ss) ^ hasher(d.cc);
    }
};

static std::unordered_map<GenData, GenData*, hash_fun> GEN_DATA_CACHE;
static std::unordered_map<bool, GenData*> BASIS_CACHE;

static GenData *NULLDATA;


GenData * setupNullData() {
    if (NULLDATA != nullptr) {
        return NULLDATA;
    }

    NULLDATA = new GenData();
    NULLDATA->data = 0;
    NULLDATA->ne = NULLDATA;
    NULLDATA->nw = NULLDATA;
    NULLDATA->se = NULLDATA;
    NULLDATA->sw = NULLDATA;
    NULLDATA->cc = NULLDATA;
    NULLDATA->ee = NULLDATA;
    NULLDATA->ww = NULLDATA;
    NULLDATA->nn = NULLDATA;
    NULLDATA->ss = NULLDATA;
    return NULLDATA;
}

GenData *getBasis(bool data) {
    auto res = BASIS_CACHE.find(data);
    if (res != BASIS_CACHE.end())
        return res->second;
    
    GenData *val = new GenData();
    val->data = data;
    val->ne = NULLDATA;
    val->nw = NULLDATA;
    val->se = NULLDATA;
    val->sw = NULLDATA;

    val->cc = NULLDATA;
    val->ee = NULLDATA;
    val->ww = NULLDATA;
    val->nn = NULLDATA;
    val->ss = NULLDATA;
    return val;
}

// Memorization of data
// returns true if new pointer created
bool getGenData(GenData **newLoc, GenData data) {
    auto res = GEN_DATA_CACHE.find(data);
    if (res != GEN_DATA_CACHE.end()) {
        *newLoc = res->second;
        return true;
    }

    GenData *val = (GenData *) malloc(sizeof(GenData));
    memcpy(val, (void *) &data, sizeof(GenData));
    *newLoc = val;
    GEN_DATA_CACHE[data] = val;
    return false;
}

// A few functions to help with life
inline bool life (
    bool nw, bool nn, bool ne,
    bool ww, bool cc, bool ee,
    bool sw, bool ss, bool se
) {
    uint count = nw + nn + ne + ww + ee + sw + ss + se;
    return (count == 2) ? cc : (count == 3);
}

// Calculate life rules for a 4x4 gen data
inline char life_4 (
    GenData *nw, GenData *ne,
    GenData *sw, GenData *se
) {
    char data;
    // Set data bits for new center
    data |= life (
                nw(nw), ne(nw), nw(ne),
                sw(nw), se(nw), sw(ne),
                nw(sw), ne(sw), nw(se)
            ) << 3; // nw

    data |= life(
                ne(nw), nw(ne), ne(ne),
                se(nw), sw(ne), se(ne),
                ne(sw), nw(se), ne(se)
            )<< 2; // ne

    data |= life(
                sw(nw), se(nw), sw(ne),
                nw(sw), ne(sw), nw(se),
                sw(sw), se(sw), sw(se)
            ) << 1; // sw 
    data |= life (
                se(nw), sw(ne), se(ne),
                ne(sw), nw(se), ne(se),
                se(sw), sw(se), se(se)
            ); // se
    return data;
}



#endif /* GAME_HPP */
