#include<stdio.h>
#include <string.h>
int main(){
        char name[20];
        char type[20];
        int tickets;
        int choice;
        float total_ticket_price;
        float discount;
        float final_price;
        const int Standard = 150;
        const int Premium = 250;
        const int Recliner = 400;
      //  printf("Enter your name(a single name without spaces): ");
        scanf("%19s", name);
       // printf("Enter number of tickets(upto 20): ");
        scanf("%d", &tickets);
        if(tickets>20 || tickets<1){
                printf("number of tickets should be in range 1-20.\n");
                return 1;
        }
       // printf("Ticket category choice (1 = Standard, 2 = Premium, 3 = Recliner): ");
        scanf("%d", &choice);
        switch(choice){
                case 1:
                        total_ticket_price = Standard*tickets;
                        strcpy(type, "Standard");
                        break;
                case 2:
                        total_ticket_price = Premium*tickets;
                        strcpy(type, "Premium");
                        break;
                case 3:
                        total_ticket_price = Recliner*tickets;
                        strcpy(type, "Recliner");
                        break;
                default:
                        printf("Invalid choice.\n");
                        return 1;
        }
        if(tickets>=5){
                discount = 0.1*total_ticket_price;
        }
        else{
                discount = 0;
        }
        final_price = total_ticket_price - discount;
        printf("Customer: %s\n", name);
        printf("Ticket Category: %s\n", type);
        printf("Tickets: %d\n", tickets);
        printf("Subtotal: %.2f\n", total_ticket_price);
        printf("Discount: %.2f\n", discount);
        printf("Final Bill: %.2f\n", final_price);
        return 0;
    }
