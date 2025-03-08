#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// Function to check if a string matches a apattern using DP

bool isMatch(const char *text, const char *pattern){
    int textlen = strlen(text);
    int patternlen=strlen(pattern);

    // DP table where dp[i][j] is true if text[0..i-1] matches pattern[0..j-1]
    bool dp[textlen+1][patternlen+1];

    // Initialize DP table
    memset(dp, false, sizeof(dp));
    dp[0][0] = true;

    // Handle patterns like a*, a*b*, a*b*c* where '*' can match elements
    for(int j=1; j<=patternlen; j++){
        if(pattern[j-1] == '*'){
            dp[0][j] = dp[0][j-2];
        }
    }

    // Fill the DP table
    for(int i=1; i<=textlen; i++){
        for(int j=1; j<=patternlen; j++){
            if(pattern[j-1] == '.' || pattern[j-1] == text[i-1]){
                // if current char match or pattern has '.', inherit th
                dp[i][j] = dp[i-1][j-1];
            }else if(pattern[j - 1] == '*'){
                // '*' can match zero of the pereceeding element 
                dp[i][j] = dp[i][j-2];
                // Or '*' can match one more of the preceding element
                if(pattern[j-2]== '.' || pattern[j-2]==text[i-1]){
                    dp[i][j] = dp[i][j] || dp[i-1][j];
                }
            }
        }
    }

    return dp[textlen][patternlen];
}





