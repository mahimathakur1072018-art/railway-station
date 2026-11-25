//2. INTERMEDIATE — RAILWAY RESERVATION SYSTEM
#include <stdio.h>

#define MAX_BOOKINGS 1000

struct Reservation
{
    int bookingID;
    char name[100];
    int trainNumber;
    int seatNumber;
    int age;
    int day, month, year;
    int status;
};

struct Reservation bookings[MAX_BOOKINGS];
int bookingCount = 0;

/* Read a whole number and retry invalid input */
int readNumber(char prompt[], int minimum, int maximum, int *value)
{
    int ch, number, valid, digits;

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);

        number = 0;
        valid = 1;
        digits = 0;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            if (ch >= '0' && ch <= '9')
            {
                digits = 1;

                if (number > maximum / 10 ||
                    (number == maximum / 10 &&
                     ch - '0' > maximum % 10))
                {
                    valid = 0;
                }
                else
                {
                    number = number * 10 + ch - '0';
                }
            }
            else
            {
                valid = 0;
            }
        }

        if (valid && digits &&
            number >= minimum && number <= maximum)
        {
            *value = number;
            return 1;
        }

        if (ch == EOF)
            return 0;

        printf("Invalid input! Enter a whole number from %d to %d.\n",
               minimum, maximum);
    }
}

/* Read a name, including spaces */
int readName(char name[])
{
    int ch, length, valid, hasLetter;

    while (1)
    {
        printf("Enter Passenger Name: ");
        fflush(stdout);

        length = 0;
        valid = 1;
        hasLetter = 0;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            if ((ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z'))
            {
                hasLetter = 1;
            }
            else if (ch != ' ' && ch != '-' &&
                     ch != '\'' && ch != '.')
            {
                valid = 0;
            }

            if (length < 99)
                name[length++] = (char)ch;
            else
                valid = 0;
        }

        name[length] = '\0';

        if (valid && hasLetter)
            return 1;

        if (ch == EOF)
            return 0;

        printf("Invalid name! Use letters, spaces, hyphens, "
               "apostrophes or periods, up to 99 characters.\n");
    }
}

/* Check calendar date, including leap years */
int validDate(int day, int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30,
                  31, 31, 30, 31, 30, 31};

    if (month < 1 || month > 12)
        return 0;

    if (year % 400 == 0 ||
        (year % 4 == 0 && year % 100 != 0))
    {
        days[1] = 29;
    }

    return day >= 1 && day <= days[month - 1];
}

/* Load saved records when the program starts */
int loadBookings(void)
{
    FILE *file;
    struct Reservation r;
    int count;

    /* a+ creates an empty file on the first run */
    file = fopen("railway_reservations.txt", "a+");

    if (file == NULL)
    {
        printf("Unable to open reservation file.\n");
        return 0;
    }

    rewind(file);

    while ((count = fscanf(file,
                           "%d %d %d %d %d %d %d %d %99[^\n]",
                           &r.bookingID,
                           &r.trainNumber,
                           &r.seatNumber,
                           &r.age,
                           &r.day,
                           &r.month,
                           &r.year,
                           &r.status,
                           r.name)) == 9)
    {
        if (bookingCount >= MAX_BOOKINGS)
        {
            printf("Too many saved records.\n");
            fclose(file);
            return 0;
        }

        bookings[bookingCount++] = r;
    }

    if (count != EOF || ferror(file))
    {
        printf("Unable to read records: file is damaged or unreadable.\n");
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

/* Save to a temporary file before replacing the original */
int saveBookings(void)
{
    FILE *file;
    int i, failed = 0;

    file = fopen("railway_temp.txt", "w");

    if (file == NULL)
    {
        printf("Unable to create temporary file.\n");
        return 0;
    }

    for (i = 0; i < bookingCount; i++)
    {
        if (fprintf(file, "%d %d %d %d %d %d %d %d %s\n",
                    bookings[i].bookingID,
                    bookings[i].trainNumber,
                    bookings[i].seatNumber,
                    bookings[i].age,
                    bookings[i].day,
                    bookings[i].month,
                    bookings[i].year,
                    bookings[i].status,
                    bookings[i].name) < 0)
        {
            failed = 1;
        }
    }

    if (fclose(file) != 0)
        failed = 1;

    if (failed)
    {
        printf("Unable to save records.\n");
        remove("railway_temp.txt");
        return 0;
    }

    /* macOS allows replacing the existing file */
    if (rename("railway_temp.txt",
               "railway_reservations.txt") != 0)
    {
        printf("Unable to update reservation file.\n");
        remove("railway_temp.txt");
        return 0;
    }

    return 1;
}

void bookTicket(void)
{
    struct Reservation r;
    int i;

    if (bookingCount >= MAX_BOOKINGS)
    {
        printf("Booking storage is full.\n");
        return;
    }

    printf("\n===== BOOK TICKET =====\n");

    if (!readName(r.name))
        return;

    if (!readNumber("Enter Passenger Age: ", 1, 120, &r.age))
        return;

    if (!readNumber("Enter Train Number: ",
                    1, 999999, &r.trainNumber))
        return;

    printf("\nEnter Journey Date:\n");

    while (1)
    {
        if (!readNumber("Day: ", 1, 31, &r.day))
            return;

        if (!readNumber("Month: ", 1, 12, &r.month))
            return;

        if (!readNumber("Year: ", 2000, 2100, &r.year))
            return;

        if (validDate(r.day, r.month, r.year))
            break;

        printf("Invalid date! Enter the journey date again.\n");
    }

    while (1)
    {
        if (!readNumber("Enter Seat Number: ",
                        1, 9999, &r.seatNumber))
            return;

        for (i = 0; i < bookingCount; i++)
        {
            if (bookings[i].status == 1 &&
                bookings[i].trainNumber == r.trainNumber &&
                bookings[i].seatNumber == r.seatNumber &&
                bookings[i].day == r.day &&
                bookings[i].month == r.month &&
                bookings[i].year == r.year)
            {
                break;
            }
        }

        if (i == bookingCount)
            break;

        printf("Seat already booked for this train and date.\n");
        printf("Please choose another seat.\n");
    }

    /* Generate the next booking ID */
    r.bookingID = 1;

    for (i = 0; i < bookingCount; i++)
    {
        if (bookings[i].bookingID >= r.bookingID)
            r.bookingID = bookings[i].bookingID + 1;
    }

    r.status = 1;
    bookings[bookingCount++] = r;

    if (!saveBookings())
    {
        bookingCount--;
        printf("Ticket was not booked.\n");
        return;
    }

    printf("\nTicket booked successfully!\n");
    printf("Your Booking ID: %d\n", r.bookingID);
    printf("Keep this ID for cancellation.\n");
}

void displayTickets(void)
{
    int i;

    printf("\n========== RESERVATIONS ==========\n");

    if (bookingCount == 0)
    {
        printf("No reservations found.\n");
        return;
    }

    for (i = 0; i < bookingCount; i++)
    {
        printf("\nBooking ID : %d\n", bookings[i].bookingID);
        printf("Name       : %s\n", bookings[i].name);
        printf("Age        : %d\n", bookings[i].age);
        printf("Train      : %d\n", bookings[i].trainNumber);
        printf("Seat       : %d\n", bookings[i].seatNumber);
        printf("Date       : %02d/%02d/%04d\n",
               bookings[i].day,
               bookings[i].month,
               bookings[i].year);

        if (bookings[i].status == 1)
            printf("Status     : Booked\n");
        else
            printf("Status     : Cancelled\n");

        printf("---------------------------------\n");
    }
}

void cancelTicket(void)
{
    int bookingID, i;

    if (!readNumber("\nEnter Booking ID to cancel: ",
                    1, 999999, &bookingID))
        return;

    for (i = 0; i < bookingCount; i++)
    {
        if (bookings[i].bookingID == bookingID)
        {
            if (bookings[i].status == 0)
            {
                printf("This ticket is already cancelled.\n");
                return;
            }

            bookings[i].status = 0;

            if (!saveBookings())
            {
                bookings[i].status = 1;
                printf("Ticket was not cancelled.\n");
                return;
            }

            printf("Ticket cancelled successfully.\n");
            return;
        }
    }

    printf("Booking ID not found.\n");
}

int main(void)
{
    int choice;

    if (!loadBookings())
        return 1;

    do
    {
        printf("\n===============================\n");
        printf("   RAILWAY RESERVATION SYSTEM\n");
        printf("===============================\n");
        printf("1. Book Ticket\n");
        printf("2. Cancel Ticket\n");
        printf("3. Display Reservations\n");
        printf("4. Exit\n");

        if (!readNumber("Enter your choice: ", 1, 4, &choice))
            break;

        switch (choice)
        {
            case 1:
                bookTicket();
                break;

            case 2:
                cancelTicket();
                break;

            case 3:
                displayTickets();
                break;

            case 4:
                printf("Thank you!\n");
                break;
        }

    } while (choice != 4);

    return 0;
}