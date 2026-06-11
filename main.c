#include <stdio.h>
#include <string.h>

int mode = 3; 

typedef struct {
    char value[50];
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
    char key_id[50];
    char key_id2[50];
} human;

typedef struct {
    WeightedWord one;
    WeightedWord two;
    WeightedWord three;
    WeightedWord four;
    WeightedWord five;
    WeightedWord six;
    WeightedWord seven;
    WeightedWord eight;
    WeightedWord none;
    WeightedWord nine;
} keys;

human scanfortokens () {
    human target;

    target.name.weight = 1;
    target.surname.weight = 1;
    target.pet.weight = 3;
    target.team.weight = 2;
    target.car.weight = 3;
    target.american_spell.weight = 1;
    target.european_spell.weight = 1;
    target.short_spell.weight = 2;

    printf("Enter name: \n");
    scanf("%49s", target.name.value);

    printf("Enter surname: \n");
    scanf("%49s", target.surname.value);

    printf("Enter pet: \n");
    scanf("%49s", target.pet.value);

    printf("Enter team: \n");
    scanf("%49s", target.team.value);

    printf("Enter car: \n");
    scanf("%49s", target.car.value);

    printf("Enter american spell: \n");
    scanf("%49s", target.american_spell.value);

    printf("Enter european spell: \n");
    scanf("%49s", target.european_spell.value);

    printf("Enter year only: \n");
    scanf("%49s", target.short_spell.value);

    printf("Enter first key identifier: \n");
    scanf("%49s", target.key_id);

    printf("Enter second key identifier: \n");
    scanf("%49s", target.key_id2);
    
    return target;
}

keys scanforkeys() {
    keys my_keys;

    strcpy(my_keys.one.value, "123");
    my_keys.one.weight = 1;

    strcpy(my_keys.two.value, "12345");
    my_keys.two.weight = 2;

    strcpy(my_keys.three.value, "123456789");
    my_keys.three.weight = 2;

    strcpy(my_keys.four.value, "!");
    my_keys.four.weight = 2;

    strcpy(my_keys.five.value, "*");
    my_keys.five.weight = 2;

    strcpy(my_keys.six.value, ".");
    my_keys.six.weight = 1;

    strcpy(my_keys.seven.value, ":");
    my_keys.seven.weight = 3;

    strcpy(my_keys.eight.value, "?");
    my_keys.eight.weight = 3;

    strcpy(my_keys.none.value, "");
    my_keys.none.weight = 1;

    strcpy(my_keys.nine.value, "321");
    my_keys.nine.weight = 2;

    return my_keys;
}

void ConvertStructHumanToField(human target, WeightedWord all_words[10]) {
    all_words[0] = target.name;
    all_words[1] = target.surname;
    all_words[2] = target.pet;
    all_words[3] = target.team;
    all_words[4] = target.car;
    all_words[5] = target.american_spell;
    all_words[6] = target.european_spell;
    all_words[7] = target.short_spell;

    strcpy(all_words[8].value, target.key_id);
    all_words[8].weight = 1;

    strcpy(all_words[9].value, target.key_id2);
    all_words[9].weight = 1;
}

void OneWordPrint(WeightedWord all_keys[10], WeightedWord all_words[10]) {
    int value;
    long long total_words = 0;
    long long total_bytes = 0;

    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 10; b++) {
            for (int c = 0; c < 10; c++) {
                value = all_keys[a].weight + (all_words[b].weight * 2) + all_keys[c].weight;
                
                int max_allowed_weight = 10;
                if (mode == 2) max_allowed_weight = 15;
                if (mode == 3) max_allowed_weight = 105;

                if (value <= max_allowed_weight) {
                    char key[200] = "";
                    strcat(key, all_keys[a].value);
                    strcat(key, all_words[b].value);
                    strcat(key, all_keys[c].value);
                    
                    // printf("%s\n", key);
                    
                    total_words++;
                    total_bytes += strlen(key) + 1;
                }
            }
        }
    }

    printf("\n--- STATS ---\n");
    printf("Total passwords: %lld\n", total_words);
    printf("Total size: %lld Bytes\n", total_bytes);
    printf("Total size: %lld bits\n", total_bytes * 8);
    if (total_words > 0) {
        double avg_bytes = (double)total_bytes / total_words;
        printf("Avg size per word: %.2f Bytes (%.2f bits)\n", avg_bytes, avg_bytes * 8);
    }
}


void TwoWordPrint(WeightedWord all_keys[10], WeightedWord all_words[10]) {
    int value;
    long long total_words = 0;
    long long total_bytes = 0;

    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 10; b++) {
            for (int c = 0; c < 10; c++) {
                for (int d = 0; d < 10; d++) {
                    for (int e = 0; e < 10; e++) {
                        value = all_keys[a].weight + (all_words[b].weight * 2) + all_keys[c].weight + (all_words[d].weight * 2) + all_keys[e].weight;
                        
                        int max_allowed_weight = 18;
                        if (mode == 2) max_allowed_weight = 20;
                        if (mode == 3) max_allowed_weight = 105;

                        if (value <= max_allowed_weight) {
                            char key[200] = "";
                            strcat(key, all_keys[a].value);
                            strcat(key, all_words[b].value);
                            strcat(key, all_keys[c].value);
                            strcat(key, all_words[d].value);
                            strcat(key, all_keys[e].value);
                            // printf("%s\n", key);
                            
                            total_words++;
                            total_bytes += strlen(key) + 1;
                        }
                    }
                }
            }
        }
    }

    printf("\n--- STATS ---\n");
    printf("Total passwords: %lld\n", total_words);
    printf("Total size: %lld Bytes\n", total_bytes);
    printf("Total size: %lld bits\n", total_bytes * 8);
    if (total_words > 0) {
        double avg_bytes = (double)total_bytes / total_words;
        printf("Avg size per word: %.2f Bytes (%.2f bits)\n", avg_bytes, avg_bytes * 8);
    }
}

void ThreeWordPrint(WeightedWord all_keys[10], WeightedWord all_words[10]) {
    int value;
    long long total_words = 0;
    long long total_bytes = 0;

    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 10; b++) {
            for (int c = 0; c < 10; c++) {
                for (int d = 0; d < 10; d++) {
                    for (int e = 0; e < 10; e++) {
                        for (int f = 0; f < 10; f++) {
                            for (int g = 0; g < 10; g++) {
                                value = all_keys[a].weight + (all_words[b].weight * 2) + all_keys[c].weight + (all_words[d].weight * 2) + all_keys[e].weight + (all_words[f].weight * 2) + all_keys[g].weight;
                                char word1[50], word2[50], word3[50];
                                strcpy(word1, all_words[b].value);
                                strcpy(word2, all_words[d].value);
                                strcpy(word3, all_words[f].value);

                                char key1[200] = "";
                                sprintf(key1, "%s%s%s%s%s%s%s", all_keys[a].value, word1, all_keys[c].value, word2, all_keys[e].value, word3, all_keys[g].value);

                                if (word1[0] >= 'a' && word1[0] <= 'z') word1[0] -= 32; 
                                if (word2[0] >= 'a' && word2[0] <= 'z') word2[0] -= 32;
                                if (word3[0] >= 'a' && word3[0] <= 'z') word3[0] -= 32;

                                char key2[200] = "";
                                sprintf(key2, "%s%s%s%s%s%s%s", all_keys[a].value, word1, all_keys[c].value, word2, all_keys[e].value, word3, all_keys[g].value);

                                for (int i = 0; word1[i] != '\0'; i++) {
                                    if (word1[i] >= 'a' && word1[i] <= 'z') word1[i] -= 32;
                                }
                                for (int i = 0; word2[i] != '\0'; i++) {
                                    if (word2[i] >= 'a' && word2[i] <= 'z') word2[i] -= 32;
                                }
                                for (int i = 0; word3[i] != '\0'; i++) {
                                    if (word3[i] >= 'a' && word3[i] <= 'z') word3[i] -= 32;
                                }

                                char key3[200] = "";
                                sprintf(key3, "%s%s%s%s%s%s%s", all_keys[a].value, word1, all_keys[c].value, word2, all_keys[e].value, word3, all_keys[g].value);

                                int max_allowed_weight = 20;
                                if (mode == 2) max_allowed_weight = 25;
                                if (mode == 3) max_allowed_weight = 100;

                                if (value <= max_allowed_weight) {
                                    printf("%s \n %s \n %s", key1, key2, key3);
                                    // printf("%s\n", key3);
                                    
                                    total_words += 3;
                                    total_bytes += (strlen(key1) + 1) + (strlen(key2) + 1) + (strlen(key3) + 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    printf("\n--- STATS ---\n");
    printf("Total passwords: %lld\n", total_words);
    printf("Total size: %lld Bytes\n", total_bytes);
    printf("Total size: %lld bits\n", total_bytes * 8);
    if (total_words > 0) {
        double avg_bytes = (double)total_bytes / total_words;
        printf("Avg size per word: %.2f Bytes (%.2f bits)\n", avg_bytes, avg_bytes * 8);
    }
}



int main() {
    printf("ZADAVEJ JENOM MALE PISMENA\n");
    human target = scanfortokens();
    keys primary_keys = scanforkeys();
    
    WeightedWord all_keys[10];
    all_keys[0] = primary_keys.one;
    all_keys[1] = primary_keys.two;
    all_keys[2] = primary_keys.three;
    all_keys[3] = primary_keys.four;
    all_keys[4] = primary_keys.five;
    all_keys[5] = primary_keys.six;
    all_keys[6] = primary_keys.seven;
    all_keys[7] = primary_keys.eight;
    all_keys[8] = primary_keys.none;
    all_keys[9] = primary_keys.nine;

    WeightedWord all_tokens[10];
    ConvertStructHumanToField(target, all_tokens);

    OneWordPrint(all_keys, all_tokens);
    TwoWordPrint(all_keys, all_tokens);
    return 0;
}