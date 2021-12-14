#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *((int*) tid);
    for(int i = 0; i<ORDERS_PER_CUSTOMER; i++){
      Order* tempOrd = (Order *)malloc(sizeof(Order));
      tempOrd->menu_item = PickRandomMenuItem();
      tempOrd->customer_id = customer_id;
      tempOrd->next = NULL;
      AddOrder(bcb,tempOrd);
    }
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *((int*) tid);
	  int orders_fulfilled = 0;
    Order* order = GetOrder(bcb);
    while(order){
      free(order);
      orders_fulfilled+=1;
      order = GetOrder(bcb);
    }
    printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
    return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
    pthread_t customerT[NUM_CUSTOMERS];
    pthread_t cookT[NUM_COOKS];
    int customerId[NUM_CUSTOMERS];
    int cookId[NUM_COOKS];
    for(int i = 0; i<NUM_CUSTOMERS; i++){
      customerId[i] = i;
      pthread_create(&customerT[i], NULL, BENSCHILLIBOWLCustomer, &customerId[i]);
    }
    for(int i = 0; i<NUM_COOKS; i++){
      cookId[i] = i;
      pthread_create(&cookT[i], NULL, BENSCHILLIBOWLCook, &cookId[i]);
    }
    for(int i = 0; i<NUM_CUSTOMERS; i++){
      pthread_join(customerT[i], NULL);
    }
    for(int i = 0; i<NUM_COOKS; i++){
      pthread_join(cookT[i], NULL);
    }
    CloseRestaurant(bcb);
    return 0;
}