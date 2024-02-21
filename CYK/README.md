# CYK Algorithm (Алгоритм Кока-Янгера-Касами)

Implementation of CYK Algorithm in C++

Input data format:
1 line: integers N, E, P - number of non-terminal symbols, number of terminal symbols and number of rule in grammar 
2 line: non-terminal symbols (capital latin letters)
3 line: terminal symbols (non-capital latin letters, digits, brackets, arithmetic operands)
next P lines: rules of grammar in format A->B (B = '' means empty terminal)
next line: integer m - number of test words
next m lines: test words 

Output data:
"Yes" or "No" for each test word

# Example:
Input data:
1 2 2
S
ab
S->aSbS
S->
S
2
aababb
aabbba

Output data:
Yes
No
