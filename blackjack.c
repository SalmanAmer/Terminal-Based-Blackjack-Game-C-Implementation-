#include "blackjack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Function Implementations


/**
 * @brief Initializes the card list by setting the head to NULL and length to 0.
 * 
 * @param cardlist Pointer to the card list to initialize.
 */
void init_cardlist(CardList *cardlist) {
    cardlist->head = NULL;
    cardlist->len = 0;
}

/**
 * @brief Fills the deck with cards (52 cards, each represented as a combination of rank and suit).
 * 
 * @param cardlist Pointer to the card list to fill.
 */
void fill_deck(CardList *cardlist) {
    init_cardlist(cardlist);
    for (int suit = 1; suit <= 8; suit <<= 1) {
        for (int rank = 1; rank <= 13; rank++) {
            Card *new_card = (Card *)malloc(sizeof(Card));
            new_card->data = (rank << 4) | suit;
            new_card->next = cardlist->head;
            cardlist->head = new_card;
            cardlist->len++;
        }
    }
}


/**
 * @brief Frees all memory associated with the card list and resets its length.
 * 
 * @param cardlist Pointer to the card list to clear.
 */
void clear_cardlist(CardList *cardlist) {
    while (cardlist->head != NULL) {
        Card *temp = cardlist->head;
        cardlist->head = cardlist->head->next;
        free(temp);
    }
    cardlist->len = 0;
}

/**
 * @brief Draws a random card from the card list.
 * 
 * @param cardlist Pointer to the card list to draw from.
 * @return Pointer to the drawn card, or NULL if the list is empty.
 */
Card *card_draw(CardList *cardlist) {
    if (cardlist->len == 0) {
        return NULL;
    }
    size_t index = rand() % cardlist->len;

    Card *prev = NULL, *current = cardlist->head;
    for (size_t i = 0; i < index; i++) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        cardlist->head = current->next;
    } else {
        prev->next = current->next;
    }

    cardlist->len--;
    current->next = NULL;
    return current;
}

/**
 * @brief Maps a suit's bitwise representation to its numeric identifier (1-4).
 * 
 * @param suit Bitwise representation of the suit.
 * @return Numeric suit value (1-4), or 0 if invalid.
 */
uint8_t suit_num(uint8_t suit){

    switch (suit){
    case 1<<0:
        return 1;
    case 1<<1:
        return 2;
    case 1<<2:
        return 3;
    case 1<<3:
        return 4;
    default:
        return 0;    
    }
}

/**
 * @brief Adds a card to the top of the card list.
 * 
 * @param cardlist Pointer to the card list.
 * @param card Pointer to the card to add.
 */
void card_push(CardList *cardlist, Card *card) {
    card->next = cardlist->head;
    cardlist->head = card;
    cardlist->len++;
}

/**
 * @brief Extracts the rank (1-13) from the card.
 * 
 * @param card Pointer to the card.
 * @return Rank of the card.
 */
uint8_t get_rank(const Card *card) {
    return (uint8_t)(card->data >> 4) & 0x0F;
}

/**
 * @brief Extracts the suit (bitwise representation) from the card.
 * 
 * @param card Pointer to the card.
 * @return Suit of the card.
 */
uint8_t get_suit(const Card *card) {
    return (uint8_t)card->data & 0x0F;
}

/**
 * @brief Prints the cards in a hand with an option to hide the second card.
 * 
 * @param label Label for the hand (e.g., "Dealer", "Player").
 * @param hand Pointer to the card list representing the hand.
 * @param hide_second_card Boolean indicating whether to hide the second card.
 */
void print_hand(const char* label, const CardList* hand, bool hide_second_card) {
    printf("%s: ", label);
    Card* current = hand->head;

    int card_index = 0;
    while (current != NULL) {
        if (hide_second_card && card_index == 1) {
            printf("??????");
        } else {
            uint8_t rank = get_rank(current);
            uint8_t suit = get_suit(current);

            // Print rank and suit using their corresponding names
            printf("%s of %s ", rank_names[rank], suit_names[suit_num(suit)]);
        }

        current = current->next;
        if (current != NULL) {
            printf(", "); // Add a comma between cards
        }

        card_index++;
    }
    printf("\n");
}


/**
 * @brief Calculates the total value of a hand, accounting for Aces being worth 1 or 11.
 * 
 * @param hand Pointer to the card list representing the hand.
 * @return Total value of the hand.
 */
int calculate_hand_value(CardList *hand) {
    int value = 0, aces = 0;
    for (Card *current = hand->head; current != NULL; current = current->next) {
        int rank = get_rank(current);
        if (rank == 1) {
            aces++;
            value += 1;
        } else if (rank > 10) {
            value += 10;
        } else {
            value += rank;
        }
    }
    while (aces > 0 && value <= 11) {/////  check here for some mistakes
        value += 10;
        aces--;
    }
    return value;
}

void game_init(GameState *state) {
    srand(time(NULL));
    fill_deck(&state->deck);
    init_cardlist(&state->player_hand);
    init_cardlist(&state->dealer_hand);
    state->cash = 1000;
    state->pot = 0;
}

void game_betting_phase(GameState *state) {
    printf("Cash: %d, Pot: %d\n", state->cash, state->pot);

    if (state->cash < 10 && state->pot == 0) {
        printf("Out of cash. Game over.\n");
        exit(0);
    }

    int bet = 0;
    do {
        printf("Enter your bet (multiples of 10): ");
        scanf("%d", &bet);
        if (bet > state->cash || bet < 0 || (bet % 10 != 0 && bet != 0)) {
            printf("Invalid bet. Try again.\n");
        }
    } while (bet > state->cash || bet < 0 || (bet % 10 != 0 && bet != 0));

    state->cash -= bet;
    state->pot += bet;
}

void game_initial_deal(GameState *state) {
    for (int i = 0; i < 2; i++) {
        card_push(&state->player_hand, card_draw(&state->deck));
        card_push(&state->dealer_hand, card_draw(&state->deck));
    }

    // Printing dealer's hand, hiding the second card
    print_hand("Dealer", &state->dealer_hand, true);

    // Printing player's hand, revealing all cards
    print_hand("Player", &state->player_hand, false);

}

void game_blackjack_check(GameState *state) {
    int player_value = calculate_hand_value(&state->player_hand);

    if (player_value == 21) {
        printf("Blackjack! You win.\n");
        state->cash += state->pot + state->pot / 2;
        state->pot = 0;
    }
}

void game_hit_or_stand(GameState *state) {
    while (1) {
        int player_value = calculate_hand_value(&state->player_hand);
        printf("Player hand value: %d\n", player_value);

        if (player_value > 21) {
            printf("Bust! You lose.\n");
            state->pot = 0;
            return;
        }

        printf("Do you want to hit or stand? (h/s): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 's') {
            break;
        } else if (choice == 'h') {
            card_push(&state->player_hand, card_draw(&state->deck));
        }
    }
}

void game_dealer_draw(GameState *state) {
    while (calculate_hand_value(&state->dealer_hand) < 17) {
        card_push(&state->dealer_hand, card_draw(&state->deck));
    }

    int dealer_value = calculate_hand_value(&state->dealer_hand);
    int player_value = calculate_hand_value(&state->player_hand);

    printf("Dealer hand value: %d\n", dealer_value);
    if (dealer_value > 21 || player_value > dealer_value) {
        printf("You win!\n");
        state->cash += 2 * state->pot;
        state->pot = 0;
    } else if (dealer_value > player_value) {
        printf("Dealer wins.\n");
        state->pot = 0;
    } else {
        printf("It's a tie.\n");
    }
}

void game_reset_cards(GameState *state) {

    while (state->player_hand.head != NULL) {
        card_push(&state->deck, card_draw(&state->player_hand));
    }
    while (state->dealer_hand.head != NULL) {
        card_push(&state->deck, card_draw(&state->dealer_hand));
    }
}

void game_run(GameState *state) {
    game_init(state);
    while (1) {
        game_betting_phase(state);
        game_initial_deal(state);
        game_blackjack_check(state);
        game_hit_or_stand(state);
        game_dealer_draw(state);
        game_reset_cards(state);

        if (state->cash < 10) {
            printf("Out of cash. Game over.\n");
            break;
        }

        printf("Do you want to play again? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y') {
            break;
        }

 
    }
}

int main() {
    GameState state;
    game_run(&state);
    return 0;
}