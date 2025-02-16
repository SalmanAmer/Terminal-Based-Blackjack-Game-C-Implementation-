#ifndef BLACKJACK_H
#define BLACKJACK_H



#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Card struct
typedef struct Card {
    uint8_t data;            // 4 bits for rank [7:4], 4 bits for suit [3:0]
    struct Card* next;       // Next card in the list
} Card;

// CardList struct
typedef struct CardList {
    Card* head;              // Pointer to the head of the list
    size_t len;              // Length of the list
} CardList;

// GameState struct
typedef struct GameState {
    CardList deck;
    CardList dealer_hand;
    CardList player_hand;
    int cash;
    int pot;
} GameState;
const char* rank_names[] = {
    "Invalid", "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"
};

const char* suit_names[] = {
    "Invalid", "Hearts", "Clubs", "Diamonds", "Spades"
};
// Functions for card and list manipulation
void init_cardlist(CardList* cardlist);
void fill_deck(CardList* deck);
void clear_cardlist(CardList* cardlist);
uint8_t get_rank(const Card* card);
uint8_t get_suit(const Card* card);
uint8_t suit_num(uint8_t suit);
Card* card_new(int rank, int suit);
void card_push(CardList* cardlist, Card* card);
Card* card_remove_at(CardList* cardlist, size_t index);
Card* card_draw(CardList* cardlist);

// Functions for game logic
void game_init(GameState* state);
void game_betting_phase(GameState* state);
void game_initial_deal(GameState* state);
void game_blackjack_check(GameState* state);
void game_hit_or_stand(GameState* state);
void game_dealer_draw(GameState* state);
void game_reset_cards(GameState* state);
void game_run(GameState* state);

// Helper functions
int calculate_hand_value(CardList* hand);
void print_card(const Card* card);
void print_hand(const char* name, const CardList* hand, bool hide_dealer_card);
int prompt_int(const char* prompt, int min, int max, int step);

#endif // BLACKJACK_H