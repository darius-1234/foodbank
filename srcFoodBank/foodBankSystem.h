#ifndef ARM11_23_FOODBANKSYSTEM_H
#define ARM11_23_FOODBANKSYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// constants for size of foods and food bank
#define MAX_FOOD_NAME_LENGTH 40
#define MAX_FRUIT_CAPACITY 500
#define MAX_CARB_ITEM_CAPACITY 1000
#define MAX_PROTEIN_ITEM_CAPACITY 800
#define MAX_INDIVIDUAL_ITEM_CAPACITY 50
#define MAX_TOTAL_CAPACITY 2500
#define MAX_NAME_LENGTH 25

#define PROTEIN_REQUIREMENT 490
#define CARBS_REQUIREMENT 750
#define FRUIT_REQUIREMENT 7

#define MAX_BOX_SIZE 50

#define NUM_OF_STAFF 4
#define NUM_OF_RECIPIENTS 50
#define MAX_STAFF_ID_LENGTH 7

/* FOOD REQUIREMENTS
*1 fruit daily (~7 in box)
* 70g protein daily (~490g in box)
* 250g carbs daily (~1750g in box)
*/

typedef struct date {
    int year;
    int month;
    int day;
} date_t;


typedef enum {
    FRUIT, CARBS, PROTEIN
} foodCategory;

typedef struct nutrientInfo {
    int proteinInGrams;
    int carbsInGrams;
} nutrientInfo_t;


typedef struct individualItem {
    // an individual item holds 2 ids. One of them is the id of the food it falls under.
    // ie every apple will have food_type_id = x where x is the food_id of apple
    // the unique_item_id is a unique number. every item in the food bank has a different id.
    int food_type_id;
    int unique_item_id;
    date_t expiryDate;
    char food_name[MAX_FOOD_NAME_LENGTH];
    foodCategory category;
    nutrientInfo_t nutrientInfo; //only defined for non-fruits
} individualItem_t;

typedef struct boxOfFood {
    individualItem_t food[MAX_BOX_SIZE];
} boxOfFood_t;

typedef struct foodItem {
    int food_id;
    int numAvailable;
    individualItem_t items[MAX_INDIVIDUAL_ITEM_CAPACITY];
} foodItem_t;

typedef enum {
    STAFF, DONOR, RECIPIENT
} userType;

// the structure we will use to model donations
typedef struct donor {
    individualItem_t *food;
} donor_t;

typedef struct recipient {
    int recipientId;
    int numItems;
    int lastDonation;
    boxOfFood_t foodReceived;
} recipient_t;

typedef struct staff {
    char cardId[MAX_STAFF_ID_LENGTH];
} staff_t;

typedef struct user {
    userType userType;
    char name[MAX_NAME_LENGTH];
    union {
        staff_t staff;
        donor_t donor;
        recipient_t recipient;
    } typeData;
} user_t;

typedef struct inventory {
    int idCount;

    foodItem_t fruitStock[MAX_FRUIT_CAPACITY];
    int numFruits; //Different types
    int sumFruits; //Total fruits

    foodItem_t carbsStock[MAX_CARB_ITEM_CAPACITY];
    int numCarbs;
    int sumGramsCarbs;

    foodItem_t proteinStock[MAX_PROTEIN_ITEM_CAPACITY];
    int numProteins;
    int sumGramsProteins;


    donor_t donationsOnHold[MAX_TOTAL_CAPACITY];
    int numDonations;
} inventory_t;


foodItem_t *initialiseFoodItem(inventory_t *inv, int numAvailable, char *name,
                               date_t expiryDate, foodCategory category,
                               int protein, int carbs);

date_t initialiseDate(int year, int month, int day);

individualItem_t *
initialiseIndividualItem(inventory_t *inv, const char *name,
                         date_t expiryDate,
                         int protein,
                         int carbs, foodCategory category);

void increaseSumGrams(inventory_t *inv, individualItem_t item);

void initialiseNutrients(individualItem_t *food, int protein, int carbs);

inventory_t *initialiseInventory();

bool printStocktype(inventory_t *inv, int numOfStock, foodItem_t *stock);

void printInventory(inventory_t *inv);

bool donate(inventory_t *inv, user_t donation);

bool addToStock(inventory_t *inv, foodItem_t *foodStock, user_t donation, int maxSpace, int *nextPos);

void printDonationsOnHold(inventory_t *inv);

user_t *createDonor(individualItem_t *foodBeingDonated, const char *name);

int hash(const char *name);

user_t *initialiseRecipient(int id, char *name, int lastDonation);

bool giveBoxOfFood(inventory_t *inv, user_t *recipient);

bool giveFoodType(inventory_t *inv, user_t *recipient, int amountRequired, int numOccurencesOfType, foodItem_t *stock,
                  foodCategory category);

bool stockLeft(foodItem_t stock[], int numOccurencesOfType);

void setHighestDate(date_t *expDate);

#endif //ARM11_23_FOODBANKSYSTEM_H


