#include <stdlib.h>
#include "../srcFoodBank/foodBankSystem.h"

int asserts_ran = 0, asserts_failed = 0, tests_ran = 0, tests_failed = 0;

#define assert_eq(a, b) do { asserts_ran++; if ((a) != (b)) { printf("%s(line %d): got: %d | expected: %d\n", __func__, __LINE__, a, b); asserts_failed++;} } while (0)

#define assert_str_eq(a, b) do { asserts_ran++; if (a == NULL || strcmp((a), (b)) != 0) { printf("%s(%d): got: %s | expected: %s\n", __func__, __LINE__, a, b); asserts_failed++; } } while (0)

#define assert_true(a) do { asserts_ran++; if (!a) { printf("%s(%d): got: false | expected: true\n", __func__, __LINE__); asserts_failed++; } } while (0)

#define assert_str_eq_not(a, b) do { asserts_ran++; if (a == NULL || strcmp((a), (b)) == 0) { printf("%s(%d): got: %s | expected: %s\n", __func__, __LINE__, a, b); asserts_failed++; } } while (0)

#define assert_false(a) do { asserts_ran++; if (a) { printf("%s(%d): got: true | expected: false\n", __func__, __LINE__); asserts_failed++; } } while (0)

#define run_test(test) do { \
asserts_ran = asserts_failed = 0; test(); \
tests_ran++;\
if (asserts_failed > 0) { \
tests_failed++; \
 printf("****%s: %d asserts failed out of %d asserts\n\n", #test, asserts_failed, asserts_ran); \
 }\
} while (0)

void initialiseDateTest() {
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    assert_eq(breadDate1.year, 2022);
    assert_eq(breadDate1.month, 8);
    assert_eq(breadDate1.day, 29);
}


void initialiseIndividualItemTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    assert_str_eq(breadDonate1->food_name, bread);
    assert_true((breadDonate1->category == CARBS));
    assert_eq(breadDonate1->food_type_id, hash(bread));
    assert_eq(breadDonate1->nutrientInfo.proteinInGrams, 7);
    assert_eq(breadDonate1->nutrientInfo.carbsInGrams, 500);
}

//Multiple of same type of food donated to the inventory
void donateSameFoodTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t breadDate2 = initialiseDate(2022, 8, 30);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *breadDonate2 = initialiseIndividualItem(inv1, bread, breadDate2, 8, 450, CARBS);
    char johnDon[] = "john";
    char tonyDon[] = "tony";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *tonyBread2 = createDonor(breadDonate2, tonyDon);
    donate(inv1, *johnBread1);
    donate(inv1, *tonyBread2);

    assert_eq(inv1->numCarbs, 1);
    assert_eq(inv1->carbsStock[0].numAvailable, 2);
    assert_str_eq(inv1->carbsStock[0].items[0].food_name, bread);
    assert_str_eq(inv1->carbsStock[0].items[1].food_name, bread);
    assert_str_eq_not(inv1->carbsStock[0].items[2].food_name, bread);
}

//Different categories of food and different types of food donated
void donateDifferentFoodTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);

    char chicken[] = "chicken";
    char beef[] = "beef";
    date_t chickenDate = initialiseDate(2022, 9, 13);
    date_t beefDate = initialiseDate(2022, 9, 19);
    individualItem_t *chickenDonate1 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *beefDonate = initialiseIndividualItem(inv1, beef, beefDate, 170, 0, PROTEIN);

    char apple[] = "apple";
    char banana[] ="banana";
    date_t appleDate = initialiseDate(2022, 8, 30);
    date_t bananaDate = initialiseDate(2022, 9, 4);
    individualItem_t *appleDonate1 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *bananaDonate1 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);


    char johnDon[] = "john";
    char danDon[] = "dan";
    char martinDon[] = "martin";
    char garyDon[] = "gary";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    user_t *johnChicken1 = createDonor(chickenDonate1, johnDon);
    user_t *garyBeef = createDonor(beefDonate, garyDon);
    user_t *johnApple1 = createDonor(appleDonate1, johnDon);
    user_t *martinBanana1 = createDonor(bananaDonate1, martinDon);

    donate(inv1, *johnBread1);
    donate(inv1, *danPasta);
    donate(inv1, *johnChicken1);
    donate(inv1, *garyBeef);
    donate(inv1, *johnApple1);
    donate(inv1, *martinBanana1);

    assert_str_eq(inv1->carbsStock[0].items[0].food_name, bread);
    assert_str_eq_not(inv1->carbsStock[0].items[1].food_name, bread);
    assert_str_eq(inv1->carbsStock[1].items[0].food_name, pasta);
    assert_str_eq(inv1->proteinStock[0].items[0].food_name, chicken);
    assert_str_eq(inv1->proteinStock[1].items[0].food_name, beef);
    assert_str_eq(inv1->fruitStock[0].items[0].food_name, apple);
    assert_str_eq(inv1->fruitStock[1].items[0].food_name, banana);

}

//There is no food available to give a box
void emptyInventoryGiveBoxTest() {
    inventory_t *inv1 = initialiseInventory();
    char steveRec[] = "steve";
    user_t *steveRecipient = initialiseRecipient(125, steveRec, 10);
    assert_false(giveBoxOfFood(inv1, steveRecipient));
}

//They have been given a box within the last five days so cannot receive one
void cannotReceiveBoxTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);
    char johnDon[] = "john";
    char danDon[] = "dan";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    donate(inv1, *johnBread1);
    donate(inv1, *danPasta);
    char steveRec[] = "steve";
    user_t *steveRecipient = initialiseRecipient(125, steveRec, 1);
    assert_false(giveBoxOfFood(inv1, steveRecipient));
}

//Items with the closest expiry date are added to the box first
void closestExpiryDateTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    char cereal[] = "cereal";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t breadDate2 = initialiseDate(2022, 8, 30);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    date_t cerealDate = initialiseDate(2023, 8, 31);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *breadDonate2 = initialiseIndividualItem(inv1, bread, breadDate2, 8, 450, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);
    individualItem_t *cerealDonate = initialiseIndividualItem(inv1, cereal, cerealDate, 35, 678, CARBS);

    char johnDon[] = "john";
    char tonyDon[] = "tony";
    char danDon[] = "dan";
    char martinDon[] = "martin";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *tonyBread2 = createDonor(breadDonate2, tonyDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    user_t *martinCereal = createDonor(cerealDonate, martinDon);
    donate(inv1, *johnBread1);
    donate(inv1, *tonyBread2);
    donate(inv1, *danPasta);
    donate(inv1, *martinCereal);

    char steveRec[] = "steve";
    user_t *steveRecipient = initialiseRecipient(125, steveRec, 6);
    assert_true(giveBoxOfFood(inv1, steveRecipient));
    assert_str_eq(steveRecipient->typeData.recipient.foodReceived.food[0].food_name, bread);
}
//Carbs and fruit will be added to the box but not fruit
void noProteinTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    char cereal[] = "cereal";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t breadDate2 = initialiseDate(2022, 8, 30);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    date_t cerealDate = initialiseDate(2023, 8, 31);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *breadDonate2 = initialiseIndividualItem(inv1, bread, breadDate2, 8, 450, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);
    individualItem_t *cerealDonate = initialiseIndividualItem(inv1, cereal, cerealDate, 35, 678, CARBS);
    char apple[] = "apple";
    char banana[] ="banana";
    char pear[] = "pear";
    date_t appleDate = initialiseDate(2022, 8, 30);
    date_t bananaDate = initialiseDate(2022, 9, 4);
    date_t pearDate = initialiseDate(2022, 9, 4);
    individualItem_t *appleDonate1 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate2 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate3 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *bananaDonate1 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *bananaDonate2 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *pearDonate1 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate2 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate3 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate4 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate5 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    char johnDon[] = "john";
    char tonyDon[] = "tony";
    char danDon[] = "dan";
    char martinDon[] = "martin";
    char garyDon[] = "gary";
    char barryDon[] = "barry";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *tonyBread2 = createDonor(breadDonate2, tonyDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    user_t *martinCereal = createDonor(cerealDonate, martinDon);
    user_t *johnApple1 = createDonor(appleDonate1, johnDon);
    user_t *tonyApple2 = createDonor(appleDonate2, tonyDon);
    user_t *danApple3 = createDonor(appleDonate3, danDon);
    user_t *martinBanana1 = createDonor(bananaDonate1, martinDon);
    user_t *garyBanana2 = createDonor(bananaDonate2, garyDon);
    user_t *barryPear1 = createDonor(pearDonate1, barryDon);
    user_t *johnPear2 = createDonor(pearDonate2, johnDon);
    user_t *tonyPear3 = createDonor(pearDonate3, tonyDon);
    user_t *danPear4 = createDonor(pearDonate4, danDon);
    user_t *martinPear5 = createDonor(pearDonate5, martinDon);
    donate(inv1, *johnBread1);
    donate(inv1, *tonyBread2);
    donate(inv1, *danPasta);
    donate(inv1, *martinCereal);
    donate(inv1, *johnApple1);
    donate(inv1, *tonyApple2);
    donate(inv1, *danApple3);
    donate(inv1, *martinBanana1);
    donate(inv1, *garyBanana2);
    donate(inv1, *barryPear1);
    donate(inv1, *johnPear2);
    donate(inv1, *tonyPear3);
    donate(inv1, *danPear4);
    donate(inv1, *martinPear5);
    char steveRec[] = "steve";
    user_t *steveRecipient = initialiseRecipient(125, steveRec, 6);
    assert_true(giveBoxOfFood(inv1, steveRecipient));
    for (int i = 0; i < steveRecipient->typeData.recipient.numItems; i++) {
        assert_false((steveRecipient->typeData.recipient.foodReceived.food[0].category == PROTEIN));
    }
}

//Tests a small inventory of food and giving boxes
void smallInventoryTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    char cereal[] = "cereal";
    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t breadDate2 = initialiseDate(2022, 8, 30);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    date_t cerealDate = initialiseDate(2023, 8, 31);
    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *breadDonate2 = initialiseIndividualItem(inv1, bread, breadDate2, 8, 450, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);
    individualItem_t *cerealDonate = initialiseIndividualItem(inv1, cereal, cerealDate, 35, 678, CARBS);
    char apple[] = "apple";
    char banana[] ="banana";
    char pear[] = "pear";
    date_t appleDate = initialiseDate(2022, 8, 30);
    date_t bananaDate = initialiseDate(2022, 9, 4);
    date_t pearDate = initialiseDate(2022, 9, 4);
    individualItem_t *appleDonate1 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate2 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate3 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *bananaDonate1 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *bananaDonate2 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *pearDonate1 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate2 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate3 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate4 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate5 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    char chicken[] = "chicken";
    char beef[] = "beef";
    char turkey[] = "turkey";
    date_t chickenDate = initialiseDate(2022, 9, 13);
    date_t beefDate = initialiseDate(2022, 9, 19);
    date_t turkeyDate = initialiseDate(2022, 8, 31);
    individualItem_t *chickenDonate1 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate2 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate3 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate4 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *beefDonate = initialiseIndividualItem(inv1, beef, beefDate, 170, 0, PROTEIN);
    individualItem_t *turkeyDonate = initialiseIndividualItem(inv1, turkey, turkeyDate, 124, 0, PROTEIN);
    char johnDon[] = "john";
    char tonyDon[] = "tony";
    char danDon[] = "dan";
    char martinDon[] = "martin";
    char garyDon[] = "gary";
    char barryDon[] = "barry";
    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *tonyBread2 = createDonor(breadDonate2, tonyDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    user_t *martinCereal = createDonor(cerealDonate, martinDon);
    user_t *johnApple1 = createDonor(appleDonate1, johnDon);
    user_t *tonyApple2 = createDonor(appleDonate2, tonyDon);
    user_t *danApple3 = createDonor(appleDonate3, danDon);
    user_t *martinBanana1 = createDonor(bananaDonate1, martinDon);
    user_t *garyBanana2 = createDonor(bananaDonate2, garyDon);
    user_t *barryPear1 = createDonor(pearDonate1, barryDon);
    user_t *johnPear2 = createDonor(pearDonate2, johnDon);
    user_t *tonyPear3 = createDonor(pearDonate3, tonyDon);
    user_t *danPear4 = createDonor(pearDonate4, danDon);
    user_t *martinPear5 = createDonor(pearDonate5, martinDon);
    user_t *johnChicken1 = createDonor(chickenDonate1, johnDon);
    user_t *tonyChicken2 = createDonor(chickenDonate2, tonyDon);
    user_t *danChicken3 = createDonor(chickenDonate3, danDon);
    user_t *martinChicken4 = createDonor(chickenDonate4, martinDon);
    user_t *garyBeef = createDonor(beefDonate, garyDon);
    user_t *barryTurkey = createDonor(turkeyDonate, barryDon);
    donate(inv1, *johnBread1);
    donate(inv1, *tonyBread2);
    donate(inv1, *danPasta);
    donate(inv1, *martinCereal);
    donate(inv1, *johnApple1);
    donate(inv1, *tonyApple2);
    donate(inv1, *danApple3);
    donate(inv1, *martinBanana1);
    donate(inv1, *garyBanana2);
    donate(inv1, *barryPear1);
    donate(inv1, *johnPear2);
    donate(inv1, *tonyPear3);
    donate(inv1, *danPear4);
    donate(inv1, *martinPear5);
    donate(inv1, *johnChicken1);
    donate(inv1, *tonyChicken2);
    donate(inv1, *danChicken3);
    donate(inv1, *martinChicken4);
    donate(inv1, *garyBeef);
    donate(inv1, *barryTurkey);

    char paulRec[] = "paul";
    char cliveRec[] = "clive";
    char daveRec[] = "dave";
    user_t *paulRecipient = initialiseRecipient(29, paulRec, 5);
    user_t *cliveRecipient = initialiseRecipient(115, cliveRec, 7);
    user_t *daveRecipient = initialiseRecipient(345, daveRec, 12);
    assert_true(giveBoxOfFood(inv1, paulRecipient));
    assert_true(giveBoxOfFood(inv1, cliveRecipient));
    assert_false(giveBoxOfFood(inv1, daveRecipient));
}

//Tests a large inventory of food and giving out boxes
void largeInventoryTest() {
    inventory_t *inv1 = initialiseInventory();
    char bread[] = "bread";
    char pasta[] = "pasta";
    char cereal[] = "cereal";
    char potatoes[] = "potatoes";
    char porridge[] = "porridge";
    char spaghetti[] = "spaghetti";
    char rice[] = "rice";

    date_t breadDate1 = initialiseDate(2022, 8, 29);
    date_t breadDate2 = initialiseDate(2022, 8, 30);
    date_t pastaDate = initialiseDate(2023, 5, 26);
    date_t cerealDate = initialiseDate(2023, 8, 31);
    date_t potatoesDate1 = initialiseDate(2022, 10, 15);
    date_t potatoesDate2 = initialiseDate(2022, 10, 22);
    date_t potatoesDate3 = initialiseDate(2022, 10, 18);
    date_t porridgeDate = initialiseDate(2022, 11, 19);
    date_t spaghettiDate = initialiseDate(2022, 9, 14);
    date_t riceDate = initialiseDate(2024, 4, 12);

    individualItem_t *breadDonate1 = initialiseIndividualItem(inv1, bread, breadDate1, 7, 500, CARBS);
    individualItem_t *breadDonate2 = initialiseIndividualItem(inv1, bread, breadDate2, 8, 450, CARBS);
    individualItem_t *pastaDonate = initialiseIndividualItem(inv1, pasta, pastaDate, 12, 400, CARBS);
    individualItem_t *cerealDonate = initialiseIndividualItem(inv1, cereal, cerealDate, 35, 678, CARBS);
    individualItem_t *potatoesDonate1 = initialiseIndividualItem(inv1, potatoes, potatoesDate1, 27, 780, CARBS);
    individualItem_t *potatoesDonate2 = initialiseIndividualItem(inv1, potatoes, potatoesDate2, 27, 480, CARBS);
    individualItem_t *potatoesDonate3 = initialiseIndividualItem(inv1, potatoes, potatoesDate3, 32, 310, CARBS);
    individualItem_t *porridgeDonate = initialiseIndividualItem(inv1, porridge, porridgeDate, 12, 440, CARBS);
    individualItem_t *spaghettiDonate = initialiseIndividualItem(inv1, spaghetti, spaghettiDate, 31, 370, CARBS);
    individualItem_t *riceDonate = initialiseIndividualItem(inv1, rice, riceDate, 33, 390, CARBS);

    char chicken[] = "chicken";
    char beef[] = "beef";
    char turkey[] = "turkey";
    char lamb[] = "lamb";
    char eggs[] = "eggs";
    char pork[] = "pork";
    char mince[] = "mince";

    date_t chickenDate = initialiseDate(2022, 9, 13);
    date_t beefDate = initialiseDate(2022, 9, 19);
    date_t turkeyDate = initialiseDate(2022, 8, 31);
    date_t lambDate = initialiseDate(2022, 9, 2);
    date_t eggsDate = initialiseDate(2022, 10, 8);
    date_t porkDate = initialiseDate(2022, 9, 29);
    date_t minceDate = initialiseDate(2022, 9, 29);

    individualItem_t *chickenDonate1 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate2 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate3 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *chickenDonate4 = initialiseIndividualItem(inv1, chicken, chickenDate, 160, 0, PROTEIN);
    individualItem_t *beefDonate = initialiseIndividualItem(inv1, beef, beefDate, 170, 0, PROTEIN);
    individualItem_t *turkeyDonate = initialiseIndividualItem(inv1, turkey, turkeyDate, 124, 0, PROTEIN);
    individualItem_t *lambDonate = initialiseIndividualItem(inv1, lamb, lambDate, 169, 0, PROTEIN);
    individualItem_t *eggsDonate1 = initialiseIndividualItem(inv1, eggs, eggsDate, 170, 12, PROTEIN);
    individualItem_t *eggsDonate2 = initialiseIndividualItem(inv1, eggs, eggsDate, 75, 14, PROTEIN);
    individualItem_t *porkDonate = initialiseIndividualItem(inv1, pork, porkDate, 210, 0, PROTEIN);
    individualItem_t *minceDonate = initialiseIndividualItem(inv1, mince, minceDate, 260, 0, PROTEIN);

    char apple[] = "apple";
    char banana[] ="banana";
    char pear[] = "pear";
    char orange[] = "orange";
    char strawberries[] = "strawberries";
    char raspberries[] = "raspberries";

    date_t appleDate = initialiseDate(2022, 8, 30);
    date_t bananaDate = initialiseDate(2022, 9, 4);
    date_t pearDate = initialiseDate(2022, 9, 4);
    date_t orangeDate = initialiseDate(2022, 9, 8);
    date_t strawberriesDate = initialiseDate(2022, 9, 3);
    date_t raspberriesDate = initialiseDate(2022, 8, 31);

    individualItem_t *appleDonate1 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate2 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *appleDonate3 = initialiseIndividualItem(inv1, apple, appleDate, 0, 3, FRUIT);
    individualItem_t *bananaDonate1 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *bananaDonate2 = initialiseIndividualItem(inv1, banana, bananaDate, 0, 2, FRUIT);
    individualItem_t *pearDonate1 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate2 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate3 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate4 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *pearDonate5 = initialiseIndividualItem(inv1, pear, pearDate, 0, 2, FRUIT);
    individualItem_t *orangeDonate1 = initialiseIndividualItem(inv1, orange, orangeDate, 0, 4, FRUIT);
    individualItem_t *orangeDonate2 = initialiseIndividualItem(inv1, orange, orangeDate, 0, 4, FRUIT);
    individualItem_t *strawberriesDonate1 = initialiseIndividualItem(inv1, strawberries, strawberriesDate, 0, 1, FRUIT);
    individualItem_t *strawberriesDonate2 = initialiseIndividualItem(inv1, strawberries, strawberriesDate, 0, 3, FRUIT);
    individualItem_t *raspberriesDonate = initialiseIndividualItem(inv1, raspberries, raspberriesDate, 0, 3, FRUIT);

    char johnDon[] = "john";
    char tonyDon[] = "tony";
    char danDon[] = "dan";
    char martinDon[] = "martin";
    char garyDon[] = "gary";
    char barryDon[] = "barry";

    user_t *johnBread1 = createDonor(breadDonate1, johnDon);
    user_t *tonyBread2 = createDonor(breadDonate2, tonyDon);
    user_t *danPasta = createDonor(pastaDonate, danDon);
    user_t *martinCereal = createDonor(cerealDonate, martinDon);
    user_t *garyPotatoes1 = createDonor(potatoesDonate1, garyDon);
    user_t *barryPotatoes2 = createDonor(potatoesDonate2, barryDon);
    user_t *johnPotatoes3 = createDonor(potatoesDonate3, johnDon);
    user_t *tonyPorridge = createDonor(porridgeDonate, tonyDon);
    user_t *danSpaghetti = createDonor(spaghettiDonate, danDon);
    user_t *martinRice = createDonor(riceDonate, martinDon);
    user_t *johnChicken1 = createDonor(chickenDonate1, johnDon);
    user_t *tonyChicken2 = createDonor(chickenDonate2, tonyDon);
    user_t *danChicken3 = createDonor(chickenDonate3, danDon);
    user_t *martinChicken4 = createDonor(chickenDonate4, martinDon);
    user_t *garyBeef = createDonor(beefDonate, garyDon);
    user_t *barryTurkey = createDonor(turkeyDonate, barryDon);
    user_t *johnLamb = createDonor(lambDonate, johnDon);
    user_t *tonyEggs1 = createDonor(eggsDonate1, tonyDon);
    user_t *danEggs2 = createDonor(eggsDonate2, danDon);
    user_t *martinPork = createDonor(porkDonate, martinDon);
    user_t *garyPork = createDonor(minceDonate, garyDon);
    user_t *johnApple1 = createDonor(appleDonate1, johnDon);
    user_t *tonyApple2 = createDonor(appleDonate2, tonyDon);
    user_t *danApple3 = createDonor(appleDonate3, danDon);
    user_t *martinBanana1 = createDonor(bananaDonate1, martinDon);
    user_t *garyBanana2 = createDonor(bananaDonate2, garyDon);
    user_t *barryPear1 = createDonor(pearDonate1, barryDon);
    user_t *johnPear2 = createDonor(pearDonate2, johnDon);
    user_t *tonyPear3 = createDonor(pearDonate3, tonyDon);
    user_t *danPear4 = createDonor(pearDonate4, danDon);
    user_t *martinPear5 = createDonor(pearDonate5, martinDon);
    user_t *garyOrange1 = createDonor(orangeDonate1, garyDon);
    user_t *barryOrange2 = createDonor(orangeDonate2, barryDon);
    user_t *johnStrawberries1 = createDonor(strawberriesDonate1, johnDon);
    user_t *tonyStrawberries2 = createDonor(strawberriesDonate2, tonyDon);
    user_t *danRaspberries = createDonor(raspberriesDonate, danDon);

    donate(inv1, *johnBread1);
    donate(inv1, *tonyBread2);
    donate(inv1, *danPasta);
    donate(inv1, *martinCereal);
    donate(inv1, *garyPotatoes1);
    donate(inv1, *barryPotatoes2);
    donate(inv1, *johnPotatoes3);
    donate(inv1, *tonyPorridge);
    donate(inv1, *danSpaghetti);
    donate(inv1, *martinRice);
    donate(inv1, *johnLamb);
    donate(inv1, *tonyEggs1);
    donate(inv1, *danEggs2);
    donate(inv1, *martinPork);
    donate(inv1, *garyPork);
    donate(inv1, *garyOrange1);
    donate(inv1, *barryOrange2);
    donate(inv1, *johnStrawberries1);
    donate(inv1, *tonyStrawberries2);
    donate(inv1, *danRaspberries);
    donate(inv1, *johnApple1);
    donate(inv1, *tonyApple2);
    donate(inv1, *danApple3);
    donate(inv1, *martinBanana1);
    donate(inv1, *garyBanana2);
    donate(inv1, *barryPear1);
    donate(inv1, *johnPear2);
    donate(inv1, *tonyPear3);
    donate(inv1, *danPear4);
    donate(inv1, *martinPear5);
    donate(inv1, *johnChicken1);
    donate(inv1, *tonyChicken2);
    donate(inv1, *danChicken3);
    donate(inv1, *martinChicken4);
    donate(inv1, *garyBeef);
    donate(inv1, *barryTurkey);

    char steveRec[] = "steve";
    char alanRec[] = "alan";
    char blankaRec[] = "blanka";
    char paulRec[] = "paul";
    char cliveRec[] = "clive";
    char daveRec[] = "dave";
    char bruceRec[] = "bruce";

    user_t *steveRecipient = initialiseRecipient(312, steveRec, 10);
    user_t *alanRecipient = initialiseRecipient(65, alanRec, 9);
    user_t *blankaRecipient = initialiseRecipient(713, blankaRec, 0);
    user_t *paulRecipient = initialiseRecipient(29, paulRec, 5);
    user_t *cliveRecipient = initialiseRecipient(115, cliveRec, 7);
    user_t *daveRecipient = initialiseRecipient(345, daveRec, 12);
    user_t *bruceRecipient = initialiseRecipient(673, bruceRec, 7);

    assert_true(giveBoxOfFood(inv1, steveRecipient));
    assert_true(giveBoxOfFood(inv1, alanRecipient));
    assert_false(giveBoxOfFood(inv1, blankaRecipient));
    assert_true(giveBoxOfFood(inv1, paulRecipient));
    assert_true(giveBoxOfFood(inv1, cliveRecipient));
    assert_true(giveBoxOfFood(inv1, daveRecipient));
    assert_false(giveBoxOfFood(inv1, bruceRecipient));
}

int main(void) {
    tests_ran = 0;
    tests_failed = 0;

    run_test(initialiseDateTest);
    run_test(initialiseIndividualItemTest);
    run_test(donateSameFoodTest);
    run_test(donateDifferentFoodTest);
    run_test(emptyInventoryGiveBoxTest);
    run_test(cannotReceiveBoxTest);
    run_test(closestExpiryDateTest);
    run_test(noProteinTest);
    run_test(smallInventoryTest);
    run_test(largeInventoryTest);

    if (tests_failed == 0) {
        printf("********************\n"
               "     WELL DONE!\n"
               "********************\n");
    } else {
        printf("****************************************************\n"
               "%d incorrect functions out of %d tested\n"
               "****************************************************\n\n",
               tests_failed, tests_ran);
    }
}
