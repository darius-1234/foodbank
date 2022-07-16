#include "foodBankSystem.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


bool donate(inventory_t *inv, user_t donor) {
    switch (donor.typeData.donor.food->category) {
        case FRUIT:
            return addToStock(inv, inv->fruitStock, donor, MAX_FRUIT_CAPACITY, &inv->numFruits);
        case CARBS:
            return addToStock(inv, inv->carbsStock, donor, MAX_CARB_ITEM_CAPACITY, &inv->numCarbs);
        case PROTEIN:
            return addToStock(inv, inv->proteinStock, donor, MAX_PROTEIN_ITEM_CAPACITY, &inv->numProteins);
        default:
            return false;
    }
}

bool printStocktype(inventory_t *inv, int numOfStock, foodItem_t *stock) {
    bool itemExists = false;
    for (int i = 0; i < numOfStock; i++) {
        for (int j = 0; j < stock[i].numAvailable; j++) {
            itemExists = true;
            printf("%s\n", stock[i].items[j].food_name);
        }
    }
    return itemExists;
}


void printInventory(inventory_t *inv) {
    bool f = printStocktype(inv, inv->numFruits, inv->fruitStock);
    bool c = printStocktype(inv, inv->numCarbs, inv->carbsStock);
    bool p = printStocktype(inv, inv->numProteins, inv->proteinStock);

    if (!f && !c && !p) {
        printf("Inventory empty.\n");
    }
}


int hash(const char *name) {
    int res = 1;
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == ' ') {
            continue;
        }
        res *= (name[i] - 64);
    }
    return res;
}


bool addToStock(inventory_t *inv, foodItem_t *foodStock, user_t donor, int maxSpace, int *nextPos) {
    if (maxSpace == *nextPos) {
        /* the inventory is FULL!! We cannot add this food. This food will have to be put in donationsOnHold
         * and numDonations incremented
        */
        inv->donationsOnHold[inv->numDonations] = donor.typeData.donor;
        inv->numDonations++;
        printf("Thank you %s. Our centre has no space right now. Your donation has been put on our"
               "hold list. Please return another day.", donor.name);
        return false;
    } else {

        for (int i = 0; i < *nextPos; i++) {
            if (foodStock[i].food_id == donor.typeData.donor.food->food_type_id) {
                foodStock[i].items[foodStock[i].numAvailable] = *donor.typeData.donor.food;
                foodStock[i].numAvailable++;
                increaseSumGrams(inv, *donor.typeData.donor.food);
                printf("Thank you, %s, for the donation. We stock lots of %s and love them!\n", donor.name,
                       donor.typeData.donor.food->food_name);
                return true;
            }
        }
        // if this for loop didn't add the food, it means it is a new type of food. we add the food item to the stock
        printf("Ah, a new type of food. We have never had %s before! ", donor.typeData.donor.food->food_name);
        foodItem_t *newFood = initialiseFoodItem(inv, 1, donor.typeData.donor.food->food_name,
                                                 donor.typeData.donor.food->expiryDate,
                                                 donor.typeData.donor.food->category,
                                                 donor.typeData.donor.food->nutrientInfo.proteinInGrams,
                                                 donor.typeData.donor.food->nutrientInfo.carbsInGrams);
        foodStock[*nextPos] = *newFood;
        (*nextPos)++;
        increaseSumGrams(inv, *donor.typeData.donor.food);
        printf("Thank you, %s, for the donation. \n", donor.name);
        return true;
    }
}

void increaseSumGrams(inventory_t *inv, individualItem_t item) {
    if (item.category == FRUIT) {
        inv->sumFruits++;
    } else {
        inv->sumGramsProteins += item.nutrientInfo.proteinInGrams;
        inv->sumGramsCarbs += item.nutrientInfo.carbsInGrams;
    }
}


// this is used to initialise an item which we have never stocked before
foodItem_t *initialiseFoodItem(inventory_t *inv, int numAvailable, char *name,
                               date_t expiryDate, foodCategory category, int protein, int carbs) {
    foodItem_t *item = malloc(sizeof(foodItem_t));
    item->food_id = hash(name);
    inv->idCount++; // ensures every type product gets a unique id
    item->numAvailable = numAvailable;
    individualItem_t *ind = initialiseIndividualItem(
            inv, name, expiryDate, protein, carbs,
            category);
    item->items[0] = *ind;
    return item;
}

// this is used to initialise a new product of a type of item we already stock (e.g., a new apple, provided we already stock apples)
individualItem_t *
initialiseIndividualItem(inventory_t *inv, const char *name,
                         date_t expiryDate,
                         int protein,
                         int carbs, foodCategory category) {
    individualItem_t *ind = malloc(sizeof(individualItem_t));
    ind->food_type_id = hash(name);
    ind->unique_item_id = inv->idCount;
    inv->idCount++;
    for (int i = 0; name[i] != '\0'; i++) {
        ind->food_name[i] = name[i];
    }

    ind->expiryDate.day = expiryDate.day;
    ind->expiryDate.month = expiryDate.month;
    ind->expiryDate.year = expiryDate.year;

    if (category == PROTEIN || category == CARBS) {
        initialiseNutrients(ind, protein, carbs);
    }
    ind->category = category;
    return ind;
}

user_t *createDonor(individualItem_t *foodBeingDonated, const char *name) {
    user_t *donor = malloc(sizeof(user_t));
    donor->typeData.donor.food = foodBeingDonated;
    for (int i = 0; name[i] != '\0'; i++) {
        donor->name[i] = name[i];
    }
    return donor;
}


void initialiseNutrients(individualItem_t *food, int protein, int carbs) {
    food->nutrientInfo.proteinInGrams = protein;
    food->nutrientInfo.carbsInGrams = carbs;
}

inventory_t *initialiseInventory() {
    inventory_t *inv = malloc(sizeof(inventory_t));
    inv->idCount = 100;
    inv->numFruits = 0;
    inv->numCarbs = 0;
    inv->numProteins = 0;
    inv->numDonations = 0;
    inv->sumFruits = 0;
    inv->sumGramsProteins = 0;
    inv->sumGramsCarbs = 0;
    return inv;
}


user_t *initialiseRecipient(int id, char *name, int lastDonation) {
    user_t *rec = malloc(sizeof(user_t));
    rec->userType = RECIPIENT;
    for (int i = 0; i < strlen(name); i++) {
        rec->name[i] = name[i];
    }
    rec->typeData.recipient.recipientId = id;

    rec->typeData.recipient.lastDonation = lastDonation;
    return rec;
}


char *categoryEnumToString(foodCategory category) {
    switch (category) {
        case FRUIT:
            return "fruit";
        case PROTEIN:
            return "protein";
        case CARBS:
            return "carbs";
        default:
            return "special food";
    }
}

date_t initialiseDate(int year, int month, int day) {
    date_t *date = malloc(sizeof(date_t));
    date->year = year;
    date->month = month;
    date->day = day;
    return *date;
}


void setHighestDate(date_t *expDate) {
    expDate->year = 9999;
    expDate->month = 12;
    expDate->day = 31;
}


bool stockLeft(foodItem_t stock[], int numOccurencesOfType) {
    for (int i = 0; i < numOccurencesOfType; i++) {
        if (stock[i].numAvailable > 0) {
            return true;
        }
    }
    return false;
}

bool giveFoodType(inventory_t *inv, user_t *recipient, int amountRequired,
                  int numOccurencesOfType, foodItem_t stock[], foodCategory category) {
    char *categoryString = categoryEnumToString(category);
    int amountInBox = 0;
    int earliestExpFoodType = 0;
    int earliestExpFoodItem = 0;
    date_t *earliestExpDate = malloc(sizeof(date_t));
    while (amountInBox < amountRequired) {
        setHighestDate(earliestExpDate);
        if (!stockLeft(stock, numOccurencesOfType)) {
            break;
        }
        for (int i = 0; i < numOccurencesOfType; i++) {
            if (stock[i].numAvailable > 0) {
                for (int j = 0; j < stock[i].numAvailable; j++) {
                    if (stock[i].items[j].expiryDate.year < earliestExpDate->year ||
                        (stock[i].items[j].expiryDate.year == earliestExpDate->year &&
                         stock[i].items[j].expiryDate.month < earliestExpDate->month) ||
                        (stock[i].items[j].expiryDate.year == earliestExpDate->year &&
                         stock[i].items[j].expiryDate.month == earliestExpDate->month &&
                         stock[i].items[j].expiryDate.day < earliestExpDate->day)) {
                        earliestExpDate->year = stock[i].items[j].expiryDate.year;
                        earliestExpDate->month = stock[i].items[j].expiryDate.month;
                        earliestExpDate->day = stock[i].items[j].expiryDate.day;
                        earliestExpFoodType = i;
                        earliestExpFoodItem = j;
                    }
                }
            }
        }
        recipient->typeData.recipient.foodReceived.food[recipient->typeData.recipient.numItems] = stock[earliestExpFoodType].items[earliestExpFoodItem];
        recipient->typeData.recipient.numItems++;
        if (stock[earliestExpFoodType].items[earliestExpFoodItem].category == PROTEIN) {
            amountInBox += stock[earliestExpFoodType].items[earliestExpFoodItem].nutrientInfo.proteinInGrams;
        }
        if (stock[earliestExpFoodType].items[earliestExpFoodItem].category == CARBS) {
            amountInBox += stock[earliestExpFoodType].items[earliestExpFoodItem].nutrientInfo.carbsInGrams;
        }
        if (stock[earliestExpFoodType].items[earliestExpFoodItem].category == FRUIT) {
            amountInBox++;
        }

        for (int k = earliestExpFoodItem; k < stock[earliestExpFoodType].numAvailable - 1; k++) {
            stock[earliestExpFoodType].items[k] = stock[earliestExpFoodType].items[k + 1];
        }
        stock[earliestExpFoodType].numAvailable--;
    }


    if (amountInBox >= amountRequired) {
        // if we have given them enough of this type of food, we stop giving them it
        printf("Here you go! A full supply of %s \n", categoryString);
        return true;
    }
    if (amountInBox == 0) {
        printf("Sorry, we have no %s left \n", categoryString);
        return false;
    }
    if (amountInBox < amountRequired) {
        // we haven't managed to give them enough food!
        if (category == FRUIT) {
            printf("Sorry, we have not managed to give you enough %s. We have given you %d pieces\n", categoryString,
                   amountInBox);
        } else {
            printf("Sorry, we have not managed to give you enough %s. We have given you %d grams\n", categoryString,
                   amountInBox);
        }
        return false;
    }
    printf("Here you go! A full supply of %s \n", categoryString);
    return true;
}


void printDate(date_t date) {
    printf("%d/%d/%d", date.day, date.month, date.year);
}

void printBox(user_t *rec) {
    printf("\nIn your box you have: \n");
    for (int i = 0; i < rec->typeData.recipient.numItems; i++) {
        printf("%s, expires: ", rec->typeData.recipient.foodReceived.food[i].food_name);
        printDate(rec->typeData.recipient.foodReceived.food[i].expiryDate);
        printf("\n");
    }
}

void printDonationsOnHold(inventory_t *inv) {
    if (inv->numDonations == 0) {
        printf("No donations on hold. \n");
    } else {
        for (int i = 0; i < inv->numDonations; i++) {
            printf("%s", inv->donationsOnHold[i].food->food_name);
        }
    }
}

bool giveBoxOfFood(inventory_t *inv, user_t *recipient) {
    // assuming the recipient is eligible for new donation
    if (recipient->typeData.recipient.lastDonation < 5 &&
        recipient->typeData.recipient.lastDonation != -1) { // -1 means they have never had food
        printf("We gave you food %d days ago! Please return in %d days and we can give you some food\n",
               recipient->typeData.recipient.lastDonation,
               5 - recipient->typeData.recipient.lastDonation);
        return false;
    }

    printf("\nLet us see what we have in stock... \n \n");
    bool fruitGiven = giveFoodType(inv, recipient, FRUIT_REQUIREMENT, inv->numFruits, inv->fruitStock, FRUIT);
    bool carbGiven = giveFoodType(inv, recipient, CARBS_REQUIREMENT, inv->numCarbs, inv->carbsStock, CARBS);
    bool proteinGiven = giveFoodType(inv, recipient, PROTEIN_REQUIREMENT, inv->numProteins, inv->proteinStock, PROTEIN);

    // if we cannot return any food AT ALL, then return false
    if (recipient->typeData.recipient.numItems == 0) {
        printf("Sorry %s, our food bank is empty. We could not give any food. Please return later. \n",
               recipient->name);
        return false;
    } else if (!(fruitGiven && carbGiven && proteinGiven)) {
        printf("Sorry we could not give you a full box this week, but we have given as much as we could! \n");
        printBox(recipient);
        recipient->typeData.recipient.lastDonation = 0; // last donation was 0 days ago because we just gave them one
        return true;
    } else {
        // set last donation date to the current date
        printf("Here you are %s, a full box of food! See you next week.\n ", recipient->name);
        recipient->typeData.recipient.lastDonation = 0; // last donation was 0 days ago because we just gave them one
        printBox(recipient);
        return true;
    }
}
