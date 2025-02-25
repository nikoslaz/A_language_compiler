#include "list.h"

alpha_token_t* root = NULL;
int tokenCounter = 1;

char* CAT[] = {
    "todo",
    "KEYWORD",
    "CONST_INT",
    "STRING",
    "IF",
    "ELSE",
    "WHILE",
    "enumerated",
    "char*",
    "INTEGER",
    "ID"
};

alpha_token_t* createTokenNode (unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, char* subcategory, unsigned int superclass){
    alpha_token_t* neoToken = (alpha_token_t*)malloc(sizeof(alpha_token_t));
    if (!neoToken) {
        printf("memory alloc problem!\n");
        return NULL;
    }

    neoToken->line = line;
    neoToken->num_token = num_token;
    neoToken->zoumi = strdup(zoumi);
    neoToken->category = category;
    neoToken->subcategory = strdup(subcategory);
    neoToken->superclass = superclass;
    neoToken->next = NULL;

    return neoToken;
}

void insertToken(alpha_token_t** root, unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, char* subcategory, unsigned int superclass){
    /*TO DO: na allaksei to unsigned int se CAT (to enum pou ftiaksame)*/
    alpha_token_t* neoToken = createTokenNode(line, num_token, zoumi, category, subcategory, superclass);
    if (!neoToken) return;

    if (*root == NULL) {
        *root = neoToken;
        return;
    }

    alpha_token_t* temp = *root;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = neoToken;
}

void deleteToken(alpha_token_t** root, unsigned int num_token){
    if (*root == NULL) return;

    alpha_token_t* temp = *root;
    alpha_token_t* prev = NULL;

    //if the node is the head of the list
    if (temp && temp->num_token == num_token) {
        *root = temp->next;
        free(temp->zoumi); 
        free(temp->subcategory);
        free(temp);
        return;
    }

    //search for the token
    while (temp && temp->num_token != num_token) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return; //not found

    prev->next = temp->next;
    free(temp->zoumi);
    free(temp);
}

alpha_token_t* searchToken(alpha_token_t* root, unsigned int num_token){
    alpha_token_t* temp = root;
    while (temp) {
        if (temp->num_token == num_token) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printTokens(alpha_token_t* root){
    //TO DO: to format na einai opws sto FAQ!
    while (root != NULL) {
        printf("%u:  #%u  \"%s\"  %s  %s  <-%s\n", root->line, root->num_token, root->zoumi, CAT[root->category], root->subcategory, CAT[root->superclass]);
        root = root->next;
    }
}

void freeTokenList(alpha_token_t** root){
    alpha_token_t* temp;
    while (*root) {
        temp = *root;
        *root = (*root)->next;
        free(temp->zoumi); 
        free(temp);
    }
}