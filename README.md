# WordlistProfiler
A custom wordlist target profiler written in C. Including combinations of 2/3 words with common special charakters
---
# How to run
Windows
```
```
Linux
```
```
# Features
## Tokens
- Name (1)
- Surname (1)
- Pet (3)
- Favourite sport teams (2)
- Car (3)
- Birthday
  - American spelling (1)
  - European spelling (1)
  - Shortend spelling (Just year, shortened year) (2) 
- Other keywords written in tokens.txt (Custom)
## Make words together
### Usual keys
- 123 (1)
- 12345 (2)
- 123456789 (2)
- ! (2)
- * (2)
- . (1)
- : (3)
- ? (3)
- None (like nothing) (1)
- 321 (2)
- (Other numbers from 1 to 8, like 12, 1234 etc.) (3)
---
Words have generated like this: USUAL KEY + WORD + USUAL KEY + WORD + USUAL KEY. Weight is done $USUALKEYWEIGHT+ WORDWEIGHT*2 + USUALKEYWEIGHT + WORDWEIGHT*2 + USUALKEYWEIGHT$
You can have leetspeak (optional).
## Weight system
In the () are written numbers, that represent weight.
### File size by points
#### 2 words
Table of weight count, possible words, expected size, no leetspeak, no custom keywords
vygeneruj jenom tabulku pro dve slova podle parametru presne. Skore pro <= 10,15,20,25. to stejne i pro 3 slova
#### 3 words
