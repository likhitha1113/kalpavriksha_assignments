#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

struct icc_player 
{
    int player_id;
    char name[51];
    int team_index;
    int role;
    int total_runs;
    float batting_average;
    float strike_rate;
    int wickets;
    float economy_rate;
    float performance_index;
    struct icc_player *next_in_role;
    struct icc_player *next_in_team;
};

struct icc_team 
{
    int team_id;
    char name[51];
    int total_players;
    float sum_strike_rate;
    int bat_count;
    float average_strike_rate;
    struct icc_player *role_head[4];
    struct icc_player *team_head;
    struct icc_player *team_tail;
};

struct merged_node 
{
    struct icc_player *player;
    struct merged_node *next;
};

struct icc_team team_array[10];
int team_total = 10;

const char *get_role_name(int role) 
{
    if (role == 1) 
    {
        return "Batsman";
    }
    if (role == 2) 
    {
        return "Bowler";
    }
    return "All-rounder";
}

int get_role_number(const char *role_text) 
{
    if (role_text[0] == 'B' && role_text[1] == 'a') 
    {
        return 1;
    }
    if (role_text[0] == 'B' && role_text[1] == 'o') 
    {
        return 2;
    }
    return 3;
}

float calculate_performance_index(int role, float batting_average, float strike_rate, int wickets, float economy_rate) 
{
    if (role == 1) 
    {
        return (batting_average * strike_rate) / 100;
    }
    if (role == 2) 
    {
        return (wickets * 2) + (100 - economy_rate);
    }
    return ((batting_average * strike_rate) / 100) + (wickets * 2);
}

int find_team_index(int team_id) 
{
    int low = 0;
    int high = team_total - 1;
    while (low <= high) 
    {
        int mid = (low + high) / 2;
        if (team_array[mid].team_id == team_id) 
        {
            return mid;
        }
        else if (team_array[mid].team_id < team_id) 
        {
            low = mid + 1;
        }
        else 
        {
            high = mid - 1;
        }
    }
    return -1;
}

void insert_player(struct icc_team *team, struct icc_player *new_player) 
{
    struct icc_player *current;
    int role = new_player->role;

    if (team->role_head[role] == NULL || team->role_head[role]->performance_index < new_player->performance_index) 
    {
        new_player->next_in_role = team->role_head[role];
        team->role_head[role] = new_player;
    } 
    else 
    {
        current = team->role_head[role];
        while (current->next_in_role != NULL && current->next_in_role->performance_index >= new_player->performance_index) 
        {
            current = current->next_in_role;
        }
        new_player->next_in_role = current->next_in_role;
        current->next_in_role = new_player;
    }

    new_player->next_in_team = NULL;
    if (team->team_head == NULL) 
    {
        team->team_head = new_player;
        team->team_tail = new_player;
    } 
    else 
    {
        team->team_tail->next_in_team = new_player;
        team->team_tail = new_player;
    }

    team->total_players++;
    if (role == 1 || role == 3) 
    {
        team->sum_strike_rate += new_player->strike_rate;
        team->bat_count++;
        team->average_strike_rate = team->sum_strike_rate / team->bat_count;
    }
}

int player_id_exists(int player_id) 
{
    int i;
    struct icc_player *current;
    for (i = 0; i < team_total; i++) 
    {
        current = team_array[i].team_head;
        while (current != NULL) 
        {
            if (current->player_id == player_id) 
            {
                return 1;
            }
            current = current->next_in_team;
        }
    }
    return 0;
}

void load_initial_data() 
{
    int i, j;
    struct icc_player *new_player;
    for (i = 0; i < team_total; i++) 
    {
        team_array[i].team_id = i + 1;
        strcpy(team_array[i].name, teams[i]);
        team_array[i].total_players = 0;
        team_array[i].sum_strike_rate = 0;
        team_array[i].bat_count = 0;
        team_array[i].average_strike_rate = 0;
        team_array[i].team_head = NULL;
        team_array[i].team_tail = NULL;
        for (j = 0; j < 4; j++) 
        {
            team_array[i].role_head[j] = NULL;
        }
    }

    for (i = 0; i < 200; i++) 
    {
        int team_index = -1;
        for (j = 0; j < team_total; j++) 
        {
            if (strcmp(players[i].team, teams[j]) == 0) 
            {
                team_index = j;
                break;
            }
        }
        if (team_index == -1) 
        {
            continue;
        }

        new_player = (struct icc_player *)malloc(sizeof(struct icc_player));
        new_player->player_id = players[i].id;
        strcpy(new_player->name, players[i].name);
        new_player->team_index = team_index;
        new_player->role = get_role_number(players[i].role);
        new_player->total_runs = players[i].totalRuns;
        new_player->batting_average = players[i].battingAverage;
        new_player->strike_rate = players[i].strikeRate;
        new_player->wickets = players[i].wickets;
        new_player->economy_rate = players[i].economyRate;
        new_player->performance_index = calculate_performance_index(new_player->role, new_player->batting_average, new_player->strike_rate, new_player->wickets, new_player->economy_rate);
        insert_player(&team_array[team_index], new_player);
    }
}

void add_player() 
{
    int team_id, team_index;
    struct icc_player *new_player;
    printf("Enter Team ID to add player: ");
    scanf("%d", &team_id);
    team_index = find_team_index(team_id);
    if (team_index == -1) 
    {
        printf("Team not found!\n");
        return;
    }
    if (team_array[team_index].total_players >= 50) 
    {
        printf("Team already has 50 players!\n");
        return;
    }

    new_player = (struct icc_player *)malloc(sizeof(struct icc_player));
    printf("Enter Player Details:\n");
    printf("Player ID: ");
    scanf("%d", &new_player->player_id);
    if (new_player->player_id < 1 || new_player->player_id > 1000) 
    {
        printf("Player ID must be between 1 and 1000!\n");
        free(new_player);
        return;
    }
    if (player_id_exists(new_player->player_id)) 
    {
        printf("Player ID already exists!\n");
        free(new_player);
        return;
    }
    printf("Name: ");
    scanf(" %50[^\n]", new_player->name);
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &new_player->role);
    if (new_player->role < 1 || new_player->role > 3) 
    {
        printf("Invalid role!\n");
        free(new_player);
        return;
    }
    printf("Total Runs: ");
    scanf("%d", &new_player->total_runs);
    printf("Batting Average: ");
    scanf("%f", &new_player->batting_average);
    printf("Strike Rate: ");
    scanf("%f", &new_player->strike_rate);
    printf("Wickets: ");
    scanf("%d", &new_player->wickets);
    printf("Economy Rate: ");
    scanf("%f", &new_player->economy_rate);

    new_player->team_index = team_index;
    new_player->performance_index = calculate_performance_index(new_player->role, new_player->batting_average, new_player->strike_rate, new_player->wickets, new_player->economy_rate);
    insert_player(&team_array[team_index], new_player);
    printf("Player added successfully to Team %s!\n", team_array[team_index].name);
}

void print_player_row(struct icc_player *p) 
{
    printf("%-5d %-25s %-12s %-7d %-6.1f %-6.1f %-5d %-5.1f %-8.2f\n", p->player_id, p->name, get_role_name(p->role), p->total_runs, p->batting_average, p->strike_rate, p->wickets, p->economy_rate, p->performance_index);
}

void display_team_players() 
{
    int team_id, team_index;
    struct icc_player *current;
    printf("Enter Team ID: ");
    scanf("%d", &team_id);
    team_index = find_team_index(team_id);
    if (team_index == -1) 
    {
        printf("Team not found!\n");
        return;
    }
    printf("Players of Team %s:\n", team_array[team_index].name);
    printf("====================================================================================\n");
    printf("%-5s %-25s %-12s %-7s %-6s %-6s %-5s %-5s %-8s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");
    current = team_array[team_index].team_head;
    while (current != NULL) 
    {
        print_player_row(current);
        current = current->next_in_team;
    }
    printf("====================================================================================\n");
    printf("Total Players: %d\n", team_array[team_index].total_players);
    printf("Average Batting Strike Rate: %.2f\n", team_array[team_index].average_strike_rate);
}

void display_teams_by_strike_rate() 
{
    int order[10];
    int i, j, temp;
    for (i = 0; i < team_total; i++) 
    {
        order[i] = i;
    }
    for (i = 0; i < team_total - 1; i++) 
    {
        for (j = 0; j < team_total - i - 1; j++) 
        {
            if (team_array[order[j]].average_strike_rate < team_array[order[j + 1]].average_strike_rate) 
            {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-4s %-20s %-12s %-12s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");
    for (i = 0; i < team_total; i++) 
    {
        struct icc_team *t = &team_array[order[i]];
        printf("%-4d %-20s %-12.2f %-12d\n", t->team_id, t->name, t->average_strike_rate, t->total_players);
    }
    printf("=========================================================\n");
}

void display_top_k() 
{
    int team_id, team_index, role, k, count;
    struct icc_player *current;
    printf("Enter Team ID: ");
    scanf("%d", &team_id);
    team_index = find_team_index(team_id);
    if (team_index == -1) 
    {
        printf("Team not found!\n");
        return;
    }
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role < 1 || role > 3) 
    {
        printf("Invalid role!\n");
        return;
    }
    printf("Enter number of players: ");
    scanf("%d", &k);
    if (k <= 0) 
    {
        printf("Invalid number!\n");
        return;
    }

    printf("Top %d %ss of Team %s:\n", k, get_role_name(role), team_array[team_index].name);
    printf("====================================================================================\n");
    printf("%-5s %-25s %-12s %-7s %-6s %-6s %-5s %-5s %-8s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");
    current = team_array[team_index].role_head[role];
    count = 0;
    while (current != NULL && count < k) 
    {
        print_player_row(current);
        current = current->next_in_role;
        count++;
    }
    printf("====================================================================================\n");
}

struct merged_node *build_team_list(int team_index, int role) 
{
    struct merged_node *head = NULL;
    struct merged_node *tail = NULL;
    struct merged_node *new_node;
    struct icc_player *current = team_array[team_index].role_head[role];
    while (current != NULL) 
    {
        new_node = (struct merged_node *)malloc(sizeof(struct merged_node));
        new_node->player = current;
        new_node->next = NULL;
        if (head == NULL) 
        {
            head = new_node;
            tail = new_node;
        } 
        else 
        {
            tail->next = new_node;
            tail = new_node;
        }
        current = current->next_in_role;
    }
    return head;
}

struct merged_node *merge_two_lists(struct merged_node *list_one, struct merged_node *list_two) 
{
    struct merged_node dummy;
    struct merged_node *tail = &dummy;
    dummy.next = NULL;

    while (list_one != NULL && list_two != NULL) 
    {
        if (list_one->player->performance_index >= list_two->player->performance_index) 
        {
            tail->next = list_one;
            list_one = list_one->next;
        } 
        else 
        {
            tail->next = list_two;
            list_two = list_two->next;
        }
        tail = tail->next;
    }
    if (list_one != NULL) 
    {
        tail->next = list_one;
    }
    else 
    {
        tail->next = list_two;
    }
    return dummy.next;
}

struct merged_node *merge_range(int low, int high, int role) 
{
    int mid;
    struct merged_node *left_list;
    struct merged_node *right_list;
    if (low == high) 
    {
        return build_team_list(low, role);
    }
    mid = (low + high) / 2;
    left_list = merge_range(low, mid, role);
    right_list = merge_range(mid + 1, high, role);
    return merge_two_lists(left_list, right_list);
}

void display_role_all_teams() 
{
    int role;
    struct merged_node *result;
    struct merged_node *current;
    struct merged_node *next_node;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role < 1 || role > 3) 
    {
        printf("Invalid role!\n");
        return;
    }
    result = merge_range(0, team_total - 1, role);

    printf("%ss of all teams:\n", get_role_name(role));
    printf("======================================================================================\n");
    printf("%-5s %-25s %-14s %-12s %-7s %-6s %-6s %-5s %-5s %-8s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("======================================================================================\n");
    current = result;
    while (current != NULL) 
    {
        struct icc_player *p = current->player;
        printf("%-5d %-25s %-14s %-12s %-7d %-6.1f %-6.1f %-5d %-5.1f %-8.2f\n", p->player_id, p->name, team_array[p->team_index].name, get_role_name(p->role), p->total_runs, p->batting_average, p->strike_rate, p->wickets, p->economy_rate, p->performance_index);
        current = current->next;
    }
    printf("======================================================================================\n");

    current = result;
    while (current != NULL) 
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}

int main() 
{
    int choice = 0;
    load_initial_data();

    while (choice != 6) {
        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) 
        {
            add_player();
        }
        else if (choice == 2) 
        {
            display_team_players();
        }
        else if (choice == 3) 
        {
            display_teams_by_strike_rate();
        }
        else if (choice == 4) 
        {
            display_top_k();
        }
        else if (choice == 5) 
        {
            display_role_all_teams();
        }
        else if (choice == 6) 
        {
            printf("Exiting...\n");
        }
        else 
        {
            printf("Invalid choice!\n");
        }
    }
    return 0;
}