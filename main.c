/*
 * passgen - Personal Password Wordlist Generator
 *
 * Copyright (C) 2026 Kryštof Ham
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MAX_WORD_LEN      50
#define ALL_KEYS_COUNT    10
#define ALL_TOKENS_COUNT  10

/* ── Deduplication hash set ──────────────────────────────────────────────── */
#define HASH_BUCKETS (1 << 22)
#define HASH_MASK    (HASH_BUCKETS - 1)

typedef struct HashNode {
    char            *str;
    struct HashNode *next;
} HashNode;

static HashNode **hash_table = NULL;

static void hash_init(void) {
    hash_table = calloc(HASH_BUCKETS, sizeof(HashNode *));
    if (!hash_table) { perror("calloc"); exit(1); }
}

static void hash_free(void) {
    for (int i = 0; i < HASH_BUCKETS; i++) {
        HashNode *n = hash_table[i];
        while (n) {
            HashNode *tmp = n->next;
            free(n->str);
            free(n);
            n = tmp;
        }
    }
    free(hash_table);
    hash_table = NULL;
}

static uint64_t fnv1a(const char *s) {
    uint64_t h = 14695981039346656037ULL;
    while (*s) { h ^= (uint8_t)*s++; h *= 1099511628211ULL; }
    return h;
}

static int hash_insert(const char *s) {
    uint64_t h   = fnv1a(s);
    uint32_t idx = (uint32_t)(h & HASH_MASK);
    for (HashNode *n = hash_table[idx]; n; n = n->next)
        if (strcmp(n->str, s) == 0) return 0;
    HashNode *node = malloc(sizeof(HashNode));
    if (!node) { perror("malloc"); exit(1); }
    node->str  = strdup(s);
    node->next = hash_table[idx];
    hash_table[idx] = node;
    return 1;
}

/* ── Progress bar ────────────────────────────────────────────────────────── */
#define BAR_WIDTH 40

static long long progress_done  = 0;
static long long progress_total = 0;
static time_t    progress_start = 0;

static void progress_draw(void) {
    double pct = (progress_total > 0)
                 ? ((double)progress_done / (double)progress_total)
                 : 0.0;
    if (pct > 1.0) pct = 1.0;
    int filled = (int)(pct * BAR_WIDTH);
    time_t elapsed = time(NULL) - progress_start;
    fprintf(stderr, "\r[");
    for (int i = 0; i < BAR_WIDTH; i++) fputc(i < filled ? '#' : '-', stderr);
    fprintf(stderr, "] %3.0f%%  %lld pw  %llds  ",
            pct * 100.0, progress_done, (long long)elapsed);
    fflush(stderr);
}

/* ── Data structures ─────────────────────────────────────────────────────── */
typedef struct {
    char value[MAX_WORD_LEN];
    int  priority;   /* 1=common, 2=medium, 3=rare — used to filter in lower modes */
} WeightedWord;

typedef struct {
    WeightedWord name, surname, pet, team, car;
    WeightedWord american_spell, european_spell, short_spell;
    char key_id[MAX_WORD_LEN];
    char key_id2[MAX_WORD_LEN];
} human;

static int       mode        = 0;
static long long total_words = 0;
static long long total_bytes = 0;
static FILE     *fptr        = NULL;

/* max token priority allowed per mode (tokens with priority > limit are skipped) */
static int token_priority_limit = 3;
/* max key priority allowed per mode */
static int key_priority_limit   = 3;

/* ── Help ────────────────────────────────────────────────────────────────── */
static void print_help(const char *prog) {
    printf(
        "\n"
        "passgen — Personal Password Wordlist Generator\n"
        "Copyright (C) 2026 Kryštof Ham  |  License: GNU GPL v3\n"
        "\n"
        "USAGE\n"
        "  %s -m <mode> [--help]\n"
        "\n"
        "REQUIRED FLAGS\n"
        "  -m <1|2|3>   Generation mode\n"
        "               1 = fast   ~10k–50k passwords,   only common tokens+keys\n"
        "               2 = medium ~50k–300k passwords,  more tokens and keys\n"
        "               3 = full   hundreds of thousands, all combinations\n"
        "\n"
        "OPTIONAL FLAGS\n"
        "  --help       Show this help and exit\n"
        "\n"
        "TOKENS ASKED AT RUNTIME  (enter all in lower case)\n"
        "  name, surname, pet, team, car\n"
        "  american_spell  e.g. 01151990\n"
        "  european_spell  e.g. 15011990\n"
        "  year only       e.g. 1990\n"
        "  key_id          first  custom identifier\n"
        "  key_id2         second custom identifier\n"
        "\n"
        "  Duplicate passwords are removed automatically.\n"
        "  Output is written to passwords.txt\n"
        "\n"
        "EXAMPLE\n"
        "  %s -m 2\n"
        "\n"
        "LICENSE\n"
        "  GNU General Public License v3 — <https://www.gnu.org/licenses/>\n"
        "\n",
        prog, prog);
}

/* ── Token input ─────────────────────────────────────────────────────────── */
static human scanfortokens(void) {
    human t;
    /* priority 1 = most likely in a password (name, surname, dates) */
    /* priority 2 = somewhat likely (team, year, key ids)            */
    /* priority 3 = less common (pet, car, long dates)               */
    t.name.priority           = 1;
    t.surname.priority        = 1;
    t.american_spell.priority = 1;
    t.european_spell.priority = 1;
    t.short_spell.priority    = 2;
    t.team.priority           = 2;
    t.key_id[0]               = '\0';
    t.key_id2[0]              = '\0';
    t.pet.priority            = 3;
    t.car.priority            = 3;

    printf("Enter name: ");           scanf("%49s", t.name.value);
    printf("Enter surname: ");        scanf("%49s", t.surname.value);
    printf("Enter pet: ");            scanf("%49s", t.pet.value);
    printf("Enter team: ");           scanf("%49s", t.team.value);
    printf("Enter car: ");            scanf("%49s", t.car.value);
    printf("Enter american spell: "); scanf("%49s", t.american_spell.value);
    printf("Enter european spell: "); scanf("%49s", t.european_spell.value);
    printf("Enter year only: ");      scanf("%49s", t.short_spell.value);
    printf("Enter first key identifier: ");  scanf("%49s", t.key_id);
    printf("Enter second key identifier: "); scanf("%49s", t.key_id2);
    return t;
}

/* ── Key table ───────────────────────────────────────────────────────────── */
/*
 * Keys = separators/suffixes appended between tokens.
 * priority 1 = very common (!  .  empty)
 * priority 2 = common      (123  321  *)
 * priority 3 = uncommon    (12345  123456789  :  ?)
 */
static void init_keys(WeightedWord all_keys[ALL_KEYS_COUNT]) {
    const char *values[]    = {"",  "!",  ".",  "123", "321", "*",
                                "12345", "123456789", ":", "?"};
    int         priorities[] = {1,   1,    1,    2,    2,    2,
                                 3,   3,           3,   3};
    for (int i = 0; i < ALL_KEYS_COUNT; i++) {
        strcpy(all_keys[i].value, values[i]);
        all_keys[i].priority = priorities[i];
    }
}

static void convert_human_to_array(human t, WeightedWord all_words[ALL_TOKENS_COUNT]) {
    all_words[0] = t.name;
    all_words[1] = t.surname;
    all_words[2] = t.pet;
    all_words[3] = t.team;
    all_words[4] = t.car;
    all_words[5] = t.american_spell;
    all_words[6] = t.european_spell;
    all_words[7] = t.short_spell;
    strcpy(all_words[8].value, t.key_id);  all_words[8].priority = 2;
    strcpy(all_words[9].value, t.key_id2); all_words[9].priority = 2;
}

/* ── Case helpers ────────────────────────────────────────────────────────── */
static void uppercase_word(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        dest[i] = (src[i] >= 'a' && src[i] <= 'z') ? src[i] - 32 : src[i];
        i++;
    }
    dest[i] = '\0';
}

static void capitalize_word(char *dest, const char *src) {
    strcpy(dest, src);
    if (dest[0] >= 'a' && dest[0] <= 'z') dest[0] -= 32;
}

/* ── Output ──────────────────────────────────────────────────────────────── */
static void print_and_count(const char *password) {
    if (fptr == NULL) return;
    if (!hash_insert(password)) return;
    fprintf(fptr, "%s\n", password);
    total_words++;
    total_bytes += strlen(password) + 1;
    progress_done++;
    if (progress_done % 5000 == 0) progress_draw();
}

/* ── Recursive generator ─────────────────────────────────────────────────── */
/*
 * Structure: key TOKEN key TOKEN key ... (alternating)
 * current_depth counts how many TOKEN slots have been filled.
 * target_depth  is how many tokens the final password has (1, 2, or 3).
 *
 * At each level:
 *   1. Pick a key (filtered by key_priority_limit)
 *   2. If at final depth → emit; else pick a token (filtered by token_priority_limit)
 *      and recurse.
 */
static void generate_rec(int current_depth, int target_depth,
                         const char *prefix,
                         WeightedWord all_keys[ALL_KEYS_COUNT],
                         WeightedWord all_words[ALL_TOKENS_COUNT]) {
    for (int k = 0; k < ALL_KEYS_COUNT; k++) {
        if (all_keys[k].priority > key_priority_limit) continue;

        char temp_buffer[1024];
        snprintf(temp_buffer, sizeof(temp_buffer), "%s%s",
                 prefix, all_keys[k].value);

        if (current_depth == target_depth) {
            /* Leaf — emit the password as-is */
            print_and_count(temp_buffer);
            continue;
        }

        /* Not a leaf — append a token and recurse */
        for (int w = 0; w < ALL_TOKENS_COUNT; w++) {
            if (all_words[w].priority > token_priority_limit) continue;

            char word_lower[MAX_WORD_LEN];
            char word_cap[MAX_WORD_LEN];
            char word_upper[MAX_WORD_LEN];

            strcpy(word_lower, all_words[w].value);
            capitalize_word(word_cap,   all_words[w].value);
            uppercase_word (word_upper, all_words[w].value);

            char next_prefix[1024];

            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_lower);
            generate_rec(current_depth + 1, target_depth, next_prefix, all_keys, all_words);

            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_cap);
            generate_rec(current_depth + 1, target_depth, next_prefix, all_keys, all_words);

            snprintf(next_prefix, sizeof(next_prefix), "%s%s", temp_buffer, word_upper);
            generate_rec(current_depth + 1, target_depth, next_prefix, all_keys, all_words);
        }
    }
}

/* ── Top-level runner ────────────────────────────────────────────────────── */
/*
 * Mode controls which tokens and keys are used (via priority limits)
 * and how many token-depths are generated.
 *
 * Mode 1: priority 1 only  → ~few thousand passwords, very fast
 * Mode 2: priority 1+2     → tens of thousands
 * Mode 3: priority 1+2+3   → hundreds of thousands (word_count 3 too)
 */
static void run_password_generator(int word_count,
                                   WeightedWord all_keys[ALL_KEYS_COUNT],
                                   WeightedWord all_words[ALL_TOKENS_COUNT]) {
    if (word_count == 3 && mode < 3) return;
    if (word_count == 2 && mode < 1) return;

    generate_rec(0, word_count, "", all_keys, all_words);
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    int help_requested = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            help_requested = 1;
        } else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -m requires an argument (1, 2, or 3).\n");
                return 1;
            }
            mode = atoi(argv[++i]);
            if (mode < 1 || mode > 3) {
                fprintf(stderr, "Error: mode must be 1, 2, or 3.\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Unknown flag: %s  (try --help)\n", argv[i]);
            return 1;
        }
    }

    if (help_requested) { print_help(argv[0]); return 0; }

    if (mode == 0) {
        fprintf(stderr, "Error: -m <mode> is required.  Run '%s --help' for usage.\n", argv[0]);
        return 1;
    }

    /* Set priority limits based on mode */
    switch (mode) {
        case 1: token_priority_limit = 1; key_priority_limit = 1; break;
        case 2: token_priority_limit = 2; key_priority_limit = 2; break;
        case 3: token_priority_limit = 3; key_priority_limit = 3; break;
    }

    printf(
        "\n"
        "╔══════════════════════════════════════════════════════╗\n"
        "║          passgen — Password Wordlist Generator        ║\n"
        "║  Copyright (C) 2026 Kryštof Ham                      ║\n"
        "║  Released under GNU General Public License v3         ║\n"
        "║  <https://www.gnu.org/licenses/>                      ║\n"
        "╚══════════════════════════════════════════════════════╝\n"
        "\n"
        "Mode: %d  |  Enter all tokens in LOWER CASE.\n\n", mode);

    human target = scanfortokens();

    WeightedWord all_keys[ALL_KEYS_COUNT];
    init_keys(all_keys);

    WeightedWord all_tokens[ALL_TOKENS_COUNT];
    convert_human_to_array(target, all_tokens);

    fptr = fopen("passwords.txt", "w");
    if (!fptr) { perror("fopen"); return 1; }

    hash_init();

    progress_total = 500000;
    progress_start = time(NULL);

    printf("\nGenerating passwords...\n");
    run_password_generator(1, all_keys, all_tokens);
    run_password_generator(2, all_keys, all_tokens);
    run_password_generator(3, all_keys, all_tokens);

    progress_total = progress_done;
    progress_draw();
    fprintf(stderr, "\n");

    fclose(fptr);
    hash_free();

    printf("\nDone! Generated %lld unique passwords (%lld bytes) → passwords.txt\n",
           total_words, total_bytes);
    return 0;
}