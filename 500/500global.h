#ifndef FGLOBAL_H_
#define FGLOBAL_H_

#define NOT 5
#define TRUMPS -1
#define HEARTS 0
#define DIAMONDS 1
#define CLUBS 2
#define SPADES 3
//#define JOKER 4
#define MISERE 5
#define NOTRUMPS -1

#define JACK 10
#define QUEEN 11
#define KING 12
#define ACE 13
#define LBOWER 14
#define RBOWER 15
#define JOKER 16

#define WAITING_FOR_PLAYERS 0
#define BIDDING 1
#define PLAYING 2
#define FINISHED 3
#define CHOOSING_TEAMS 4
#define QUITTING 5
#define KITTY 6

#define MALCOLMVSMALCOLMSDAD -1

inline int encode(int suit, int num) {
    return suit*17+num;
}

inline int getsuit(int h) {
	return h/17;
}

inline int getnum(int h) {
	return h%17;
}

inline int encodebid(int suit, int num) {
	return (suit!=MISERE)?(2*(num*5+(3-suit))):(encodebid(SPADES,8)+MALCOLMVSMALCOLMSDAD);
}

inline int getbidsuit(int a) {
	return (a%2==1)?NOTRUMPS:(3-((a/2)%5));
}

inline int getbidnum(int a) {
	return (a%2==1)?0:(a/10);
}

#endif
