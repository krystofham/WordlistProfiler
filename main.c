#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WORD_LEN 50
#define ALL_KEYS_COUNT 10
#define ALL_TOKENS_COUNT 10


int mode;


typedef struct {
    char value[MAX_WORD_LEN];
    int weight;
} WeightedWord;

typedef struct {
    WeightedWord name;
    WeightedWord surname;
    WeightedWord pet;
    WeightedWord team;
    WeightedWord car;
    WeightedWord american_spell;
    WeightedWord european_spell;
    WeightedWord short_spell;
    char key_id[MAX_WORD_LEN];
    char key_id2[MAX_WORD_LEN];
} human;

long long total_words = 0;
long long total_bytes = 0;

FILE *fptr = NULL;

human scanfortokens() {
    human target;
    target.name.weight = 1;
    target.surname.weight = 1;
    target.pet.weight = 3;
    target.team.weight = 2;
    target.car.weight = 3;
    target.american_spell.weight = 1;
    target.european_spell.weight = 1;
    target.short_spell.weight = 2;

    printf("Enter name: \n"); scanf("%49s", target.name.value);
    printf("Enter surname: \n"); scanf("%49s", target.surname.value);
    printf("Enter pet: \n"); scanf("%49s", target.pet.value);
    printf("Enter team: \n"); scanf("%49s", target.team.value);
    printf("Enter car: \n"); scanf("%49s", target.car.value);
    printf("Enter american spell: \n"); scanf("%49s", target.american_spell.value);
    printf("Enter european spell: \n"); scanf("%49s", target.european_spell.value);
    printf("Enter year only: \n"); scanf("%49s", target.short_spell.value);
    printf("Enter first key identifier: \n"); scanf("%49s", target.key_id);
    printf("Enter second key identifier: \n"); scanf("%49s", target.key_id2);

    return target;
}

void init_keys(WeightedWord all_keys[ALL_KEYS_COUNT]) {
    char *values[] = {"123", "12345", "123456789", "!", "*", ".", ":", "?", "", "321"};
    int weights[] = {1, 2, 2, 2, 2, 1, 3, 3, 1, 2};
    for (int i = 0; i < ALL_KEYS_COUNT; i++) {
        strcpy(all_keys[i].value, values[i]);
        all_keys[i].weight = weights[i];
    }
}

void convert_human_to_array(human target, WeightedWord all_words[ALL_TOKENS_COUNT]) {
    all_words[0] = target.name;
    all_words[1] = target.surname;
    all_words[2] = target.pet;
    all_words[3] = target.team;
    all_words[4] = target.car;
    all_words[5] = target.american_spell;
    all_words[6] = target.european_spell;
    all_words[7] = target.short_spell;
    strcpy(all_words[8].value, target.key_id);   all_words[8].weight = 1;
    strcpy(all_words[9].value, target.key_id2);  all_words[9].weight = 1;
}

void uppercase_word(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        if (src[i] >= 'a' && src[i] <= 'z') {
            dest[i] = src[i] - 32;
        } else {
            dest[i] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
}

void capitalize_word(char *dest, const char *src) {
    strcpy(dest, src);
    if (dest[0] >= 'a' && dest[0] <= 'z') {
        dest[0] -= 32;
    }
}

void print_and_count(const char *password) {
    if (fptr != NULL) {
        fprintf(fptr, "%s\n", password);
        total_words++;
        total_bytes += strlen(password) + 1;
    }
}

void generate_rec(int current_depth, int target_depth, int current_weight, int max_weight, const char *prefix, WeightedWord all_keys[ALL_KEYS_COUNT], WeightedWord all_words[ALL_TOKENS_COUNT]) {
    for (int k = 0; k < ALL_KEYS_COUNT; k++) {
        int weight_with_key = current_weight + all_keys[k].weight;
        
        char temp_buffer[1024];
        snprintf(temp_buffer, sizeof(temp_buffer), "%s%s", prefix, all_keys[k].value);
        
        if (current_depth == target_depth) {
            if (weight_with_key <= max_weight) {
                print_and_count(temp_buffer);
            }
            continue; 
        }
        
        for (int w = 0; w < ALL_TOKENS_COUNT; w++) {
            int next_weight = weight_with_key + (all_words[w].weight * 2);
            if (next_weight > max_weight) continue;
            
            char word_lower[MAX_WORD_LEN];
            char word_cap[MAX_WORD_LEN];
            char word_upper[MAX_WORD_LEN];
            
            strcpy(word_lower, all_words[w].value);
            capitalize_word(word_cap, all_words[w].value);
            uppercase_word(word_upper, all_words[w].value);
            
            char next_prefix[1024];
            
            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_lower);
            generate_rec(current_depth + 1, target_depth, next_weight, max_weight, next_prefix, all_keys, all_words);
            
            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_cap);
            generate_rec(current_depth + 1, target_depth, next_weight, max_weight, next_prefix, all_keys, all_words);
            
            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_upper);
            generate_rec(current_depth + 1, target_depth, next_weight, max_weight, next_prefix, all_keys, all_words);
        }
    }
}

void run_password_generator(int word_count, WeightedWord all_keys[ALL_KEYS_COUNT], WeightedWord all_words[ALL_TOKENS_COUNT]) {
    int max_allowed_weight = 100;
    
    if (word_count == 1) {
        max_allowed_weight = (mode == 2) ? 15 : ((mode == 3) ? 20 : 10);
    } else if (word_count == 2) {
        max_allowed_weight = (mode == 2) ? 20 : ((mode == 3) ? 25 : 18);
    } else if (word_count == 3) {
        max_allowed_weight = (mode == 2) ? 25 : ((mode == 3) ? 30 : 20);
    }
    
    generate_rec(0, word_count, 0, max_allowed_weight, "", all_keys, all_words);
}

int main() {
    printf("Enter generator mode (e.g. 1/2/3): \n");
    if (scanf("%d", &mode) != 1) {
        printf("Neplatný vstup!\n");
        return 1;
    }

    printf("Enter lower case only\n");
    human target = scanfortokens();
    
    WeightedWord all_keys[ALL_KEYS_COUNT];
    init_keys(all_keys);
    
    WeightedWord all_tokens[ALL_TOKENS_COUNT];
    convert_human_to_array(target, all_tokens);
    
    fptr = fopen("passwords.txt", "w");
    if (fptr == NULL) {
        printf("Chyba při otevírání souboru pro zápis!\n");
        return 1;
    }
    
    printf("Generuji hesla...\n");
    run_password_generator(1, all_keys, all_tokens);
    run_password_generator(2, all_keys, all_tokens);
    run_password_generator(3, all_keys, all_tokens);
    
    fclose(fptr);
    
    printf("Hotovo! Vygenerováno %lld hesel (%lld bajtů).\n", total_words, total_bytes);
    
    return 0;
}