#include<stdio.h>
int main(){
        char name[20];
        int tickets;
        int choice;
        float subtotal;
        float discount;
        float final_price;
        const int Standard = 150;
        const int Premium = 250;
        const int Recliner = 400;
        printf("Enter your name(a single name without spaces): ");
        scanf("%s", name);
        printf("Enter number of tickets(upto 20): ");
        scanf("%d", &tickets);
        if(tickets>20 || tickets<1){
                printf("number of tickets should be in range 1-20.\n");
                return 0;
        }
        printf("Ticket category choice (1 = Standard, 2 = Premium, 3 = Recliner): ");
        scanf("%d", &choice);
        switch(choice){
                case 1:
                        subtotal = Standard*tickets;
                        break;
                case 2:
                        subtotal = Premium*tickets;
                        break;
                case 3:
                        subtotal = Recliner*tickets;
                        break;
                default:
                        printf("Invalid choice.\n");
                        return 0;
        }
        if(tickets>=5){
                discount = 0.1*subtotal;
        }
        else{
                discount = 0;
        }
        final_price = subtotal - discount;
        printf("Customer: %s\n", name);
        printf("Ticket category: ");
        switch(choice){
                case 1:
                        printf("Standard\n");
                        break;
                case 2:
                        printf("Premium\n");
                        break;
                case 3:
                        printf("Recliner\n");
                        break;
        }
        printf("Tickets: %d\n", tickets);
        printf("Subtotal: %.2f\n", subtotal);
        printf("Discount: %.2f\n", discount);
        printf("Final bill: %.2f\n", final_price);

        return 0;
    }
    