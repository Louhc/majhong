#include "table.h"
#include "player.h"
#include "types.h"

#include <algorithm>
#include <cassert>
#include <thread>

Table::Table() {
    wall.clear();
    for ( int i = 0; i < 136; ++i )
        wall.push_back((Tile)i);
    std::random_shuffle(wall.begin(), wall.end());
    // 0 - 121, 122 - 125, 126 - 135
    TileList player0_hand(wall.begin(), wall.begin() + 13),
             player1_hand(wall.begin() + 13, wall.begin() + 26),
             player2_hand(wall.begin() + 26, wall.begin() + 39),
             player3_hand(wall.begin() + 39, wall.begin() + 52);
    wall_pointer = 52; kan_counts = 0; current_player = 0; is_started = false;

    players[0] = new Player();
    players[1] = new Player();
    players[2] = new Player();
    players[3] = new Player();
}

Table::~Table() {
    delete players[0];
    delete players[1];
    delete players[2];
    delete players[3];
}

int Table::getPosition( const Player *who ) const {
    if ( who == players[0] ) return 0;
    else if ( who == players[1] ) return 1;
    else if ( who == players[2] ) return 2;
    else if ( who == players[3] ) return 3;
    return -1;
}

void Table::start() {
    if ( is_started ) return;
    is_started = true;

    // int ;
    int action = 0;
    Tile tile;
    while ( 1 ){
        tile = wall[wall_pointer];

        int action = players[current_player]->handleDraw(tile);
        while ( action == 140 ) {
            assert(players[current_player]->canAnKan(tile));
            // handle AnKan
        }

        if ( action == 136 ) {
            // handle win
            return;
        }
        assert(action < 136);

        Tile discard = Tile(action);
        int action1, action2, action3;
        Player *p1 = players[(current_player + 1) % 4],
                *p2 = players[(current_player + 2) % 4],
                *p3 = players[(current_player + 3) % 4];
        if ( p1->canChi(discard) || p1->canPon(discard) || p1->canKan(discard) || p1->canWin(discard) ) {
            action1 = p1->handleLeftPlayerDiscard(discard);
        } else {
            action1 = 141;
        }
        if ( p2->canPon(discard) || p2->canKan(discard) || p2->canWin(discard) ) {
            action2 = p2->handleLeftPlayerDiscard(discard);
        } else {
            action2 = 141;
        }
        if ( p3->canPon(discard) || p3->canKan(discard) || p3->canWin(discard) ) {
            action3 = p3->handleLeftPlayerDiscard(discard);
        } else {
            action3 = 141;
        }
        // handle discard
        if ( action1 == 136 || action2 == 136 || action3 == 136 ) {
            // handle win
            return;
        }
        if ( action1 == 138 || action1 == 139 ) {
            ;
        }

        wall_pointer++;
        current_player = (current_player + 1) % 4;
    }
}