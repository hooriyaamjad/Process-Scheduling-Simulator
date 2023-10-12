#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

struct job
{
    int id;
    int arrival;
    int length;
    struct job *next;
};

/*** Globals ***/
int seed = 100;

// This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

typedef struct {
    int id;
    int response_time;
    int turnaround_time;
    int wait_time;
} JobResult;

JobResult results[11];  // Assuming there are 11 jobs maximum

/*Function to append a new job to the list*/
void append(int id, int arrival, int length)
{
    // create a new struct and initialize it with the input data
    struct job *tmp = (struct job *)malloc(sizeof(struct job));

    // tmp->id = numofjobs++;
    tmp->id = id;
    tmp->length = length;
    tmp->arrival = arrival;

    // the new job is the last job
    tmp->next = NULL;

    // Case: job is first to be added, linked list is empty
    if (head == NULL)
    {
        head = tmp;
        return;
    }

    struct job *prev = head;

    // Find end of list
    while (prev->next != NULL)
    {
        prev = prev->next;
    }

    // Add job to end of list
    prev->next = tmp;
    return;
}

/*Function to read in the workload file and create job list*/
void read_workload_file(char *filename)
{
    int id = 0;
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    char *line = NULL,
         *arrival = NULL,
         *length = NULL;

    struct job **head_ptr = malloc(sizeof(struct job *));

    if ((fp = fopen(filename, "r")) == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) > 1)
    {
        arrival = strtok(line, ",\n");
        length = strtok(NULL, ",\n");

        // Make sure neither arrival nor length are null.
        assert(arrival != NULL && length != NULL);

        append(id++, atoi(arrival), atoi(length));
    }

    fclose(fp);

    // Make sure we read in at least one job
    assert(id > 0);

    return;
}

// Function to swap data of two nodes a and b
void swap(struct job *a, struct job *b)
{
    int temp = a->id;
    a->id = b->id;
    b->id = temp;

    temp = a->arrival;
    a->arrival = b->arrival;
    b->arrival = temp;

    temp = a->length;
    a->length = b->length;
    b->length = temp;
}

// Bubble sort the linked list based on job length.
// If job lengths are the same, sort by arrival time.
void sortJobsByLength(struct job **head_ref)
{
    int swapped;
    struct job *ptr1;
    struct job *lptr = NULL;

    if (*head_ref == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = *head_ref;

        while (ptr1->next != lptr)
        {
            if (ptr1->length > ptr1->next->length ||
                (ptr1->length == ptr1->next->length && ptr1->arrival > ptr1->next->arrival))
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

void policy_FIFO(struct job *head)
{
    struct job *current = head;
    int time = 0;

    printf("Execution trace with FIFO:\n");

    while (current != NULL)
    {
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, current->id, current->arrival, current->length);
        time += current->length;
        current = current->next;
    }

    printf("End of execution with FIFO.\n");
}

void analyze_FIFO(struct job *head)
{
    struct job *current = head;
    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int total_jobs = 0;
    int current_time = 0; // Initialize current time to 0

    while (current != NULL)
    {
        int response_time = current_time - current->arrival;                     // Response time is Ts - Ta
        int turnaround_time = current_time + current->length - current->arrival; // Turnaround time is Tc - Ta
        int wait_time = response_time;                                           // Wait time is Response time

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               current->id, response_time, turnaround_time, wait_time);

        total_response_time += response_time;
        total_turnaround_time += turnaround_time;
        total_wait_time += wait_time;
        total_jobs++;

        current_time += current->length; // Update current time
        current = current->next;
    }

    if (total_jobs > 0)
    {
        float avg_response_time = (float)total_response_time / total_jobs;
        float avg_turnaround_time = (float)total_turnaround_time / total_jobs;
        float avg_wait_time = (float)total_wait_time / total_jobs;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
               avg_response_time, avg_turnaround_time, avg_wait_time);
    }
    else
    {
        printf("No jobs to analyze.\n");
    }
}

void policy_SJF(struct job *head)
{
    sortJobsByLength(&head); // Sort the jobs by length

    struct job *current = head;
    int time = 0;

    printf("Execution trace with SJF:\n");

    while (current != NULL)
    {
        // Handle the case where the CPU is idle
        if (time < current->arrival)
        {
            time = current->arrival;
        }
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, current->id, current->arrival, current->length);
        time += current->length;
        current = current->next;
    }

    printf("End of execution with SJF.\n");
}

void analyze_SJF(struct job *head)
{
    struct job *current = head;
    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int total_jobs = 0;
    int current_time = 0; // Initialize current time to 0

    while (current != NULL)
    {
        int response_time = current_time - current->arrival;                     // Response time is Ts - Ta
        int turnaround_time = current_time + current->length - current->arrival; // Turnaround time is Tc - Ta
        int wait_time = response_time;                                           // Wait time is Response time

        results[current->id].id = current->id;
        results[current->id].response_time = response_time;
        results[current->id].turnaround_time = turnaround_time;
        results[current->id].wait_time = wait_time;

        total_response_time += response_time;
        total_turnaround_time += turnaround_time;
        total_wait_time += wait_time;
        total_jobs++;

        current_time += current->length; // Update current time
        current = current->next;
    }

    // Now, print the results in order:
    for (int i = 0; i < total_jobs; i++)
    {
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               results[i].id, results[i].response_time, results[i].turnaround_time, results[i].wait_time);
    }

    if (total_jobs > 0)
    {
        float avg_response_time = (float)total_response_time / total_jobs;
        float avg_turnaround_time = (float)total_turnaround_time / total_jobs;
        float avg_wait_time = (float)total_wait_time / total_jobs;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
               avg_response_time, avg_turnaround_time, avg_wait_time);
    }
    else
    {
        printf("No jobs to analyze.\n");
    }
}


int main(int argc, char **argv)
{

    if (argc < 4)
    {
        fprintf(stderr, "missing variables\n");
        fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int analysis = atoi(argv[1]);
    char *policy = argv[2],
         *workload = argv[3];

    // Note: we use a global variable to point to
    // the start of a linked-list of jobs, i.e., the job list
    read_workload_file(workload);

    if (strcmp(policy, "FIFO") == 0)
    {
        policy_FIFO(head);
        if (analysis)
        {
            printf("Begin analyzing FIFO:\n");
            analyze_FIFO(head);
            printf("End analyzing FIFO.\n");
        }
    }
    else if (strcmp(policy, "SJF") == 0)
    {
        policy_SJF(head);
        if (analysis)
        {
            printf("Begin analyzing SJF:\n");
            analyze_SJF(head);
            printf("End analyzing SJF.\n");
        }

        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
