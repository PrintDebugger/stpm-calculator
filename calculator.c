/*  Calculator for STPM grades */

#include <stdio.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

//  Subject code and name
typedef struct {
    int code;
    const char *name;
    int marks[5];
    int altPaper[2];    //  some subjects have optional papers, student can choose 1 out of 2
} SubjectInfo;

// oh god send help
const SubjectInfo subjectList[] = {
    //  the paper does not exist if marks is 0
    //  if there are no optional papers, altPaper = {0,0}
    {900, "Pengajian Am", {80,60,80,55,0}, {0,0}},
    {910, "Bahasa Melayu", {100,60,65,25,50}, {0,0}},   
    {911, "Bahasa Cina", {80,80,80,0,0}, {0,0}},  
    {912, "Bahasa Tamil", {100,100,100,0,0}, {0,0}},
    {913, "Bahasa Arab", {70,70,50,30,30}, {0,0}},
    {920, "Literature in English", {75,75,75,0,0}, {0,0}},
    {922, "Kesusteraan Melayu Komunikatif", {80,80,80,60,0}, {0,0}},
    {930, "Syariah", {80,80,80,60,0}, {0,0}},
    {931, "Usuluddin", {80,80,80,60,0}, {0,0}},
    {932, "Tahfiz Al-Quran", {60,60,70,100,0}, {0,0}},
    {940, "Sejarah", {80,60,80,55,0}, {0,0}},
    {942, "Geografi", {80,80,40,50,50}, {4,5}},
    {944, "Ekonomi", {80,80,40,50,50}, {4,5}},
    {946, "Pengajian Perniagaan", {100,100,100,100,0}, {0,0}},
    {948, "Perakaunan", {80,80,40,40,40}, {4,5}},
    {950, "Mathematics M", {60,60,60,45,0}, {0,0}},
    {954, "Mathematics T", {60,60,60,45,0}, {0,0}},
    {958, "Information And Communications Technology", {50,50,50,100,0}, {0,0}},
    {960, "Physics", {60,60,60,45,45}, {4,5}},
    {962, "Chemistry", {60,60,60,45,45}, {4,5}},
    {964, "Biology", {60,60,60,45,45}, {4,5}},
    {966, "Sains Sukan", {60,60,60,60,60}, {4,5}},
    {970, "Seni Visual", {80,70,50,50,0}, {3,4}}
};

const int totalSubjects = sizeof(subjectList) / sizeof(subjectList[0]);

//  Grade data
enum Grade {
    GRED_A, GRED_AM,
    GRED_BP, GRED_B, GRED_BM,
    GRED_CP, GRED_C, GRED_CM,
    GRED_DP, GRED_D,
    GRED_F,
    TIADA
};

const char* gradeNames[] = {"A", "A-", "B+", "B", "B-", "C+", "C", "C-", "D+", "D", "F", "?"};
const float gpa[] = {4.00, 3.67, 3.33, 3.00, 2.67, 2.33, 2.00, 1.67, 1.33, 1.00, 0.00, 0.00};

//  Function prototypes
int getSubjectIndex(int);
int subjectExists(int, int);
int changeSubject(int);
enum Grade determineGrade(float);
void displayTable();
void clearInput();

//  Score data
int numSubjects;
struct SubjectEntry {
    int subjectIndex;
    float myScore[5];       //  Stores raw scores (scaled if needed) for each paper
    int chosenOptional;     //  The optional paper taken (if any, otherwise this is -1)
};
struct SubjectEntry subjects[5];

int main() {
    printf("Grade Calculator for STPM 0.9.2\n");
    printf("by Low Jun Xuan\n");
    printf("E-mail: lowjunxuan06@gmail.com\n");
    printf("29 June 2025\n\n");

    while (numSubjects < 4 || numSubjects > 5) {
        printf("Enter number of subjects include Pengajian Am (4-5): ");
        scanf("%d", &numSubjects);
        clearInput();
    }

    //  Subject registration
    int n;
    subjects[0].subjectIndex = getSubjectIndex(900);   // set first subject to Pengajian Am
    subjects[0].chosenOptional = -1;
    for (n = 0; n < 5; n++) {
        subjects[0].myScore[n] = -1;
    }

    n = 1;
    int status;
    while (n < numSubjects) {
        subjects[n].subjectIndex = -1;
        status = changeSubject(n);  //  Returns 0 if complete, and -1 if canceled
        if (status == 0) {
            n++;
        }
    }

    displayTable();

    int ans;
    while (1) {
        ans = -1;
        printf("\n [1] Edit Marks\n [2] Change Subject\n [3] Close program\n\n");

        while (ans < 1 || ans > 3) {
            printf("Select an action (1-3): ");
            scanf("%d", &ans);
            clearInput();
        }

        if (ans == 1) {
            ans = -1;
            for (n = 0; n < numSubjects; n++) {
                printf("\n\t[%d] %s", n+1, subjectList[subjects[n].subjectIndex].name);
            }
            printf("\n\t[0] Go Back\n\n");

            while (ans < 0 || ans > numSubjects) {
                printf("Select a subject (1-%d): ", numSubjects);
                scanf("%d", &ans);
                clearInput();
            };

            if (ans == 0) {
                continue;
            }

            int selected = ans - 1, ignore = 5, totalPapers = 0;
            float thisScore, maxMarks;
            SubjectInfo currentSubject = subjectList[subjects[selected].subjectIndex];

            n = 0;
            while (currentSubject.marks[n] > 0 && n < 5) {
                thisScore = subjects[selected].myScore[n];
                maxMarks = currentSubject.marks[n];

                if (subjects[selected].chosenOptional != -1) {
                    ignore = currentSubject.altPaper[(subjects[selected].chosenOptional - 1) ^ 1] - 1;
                } else {
                    ignore = 5;
                }

                if (ignore != n) {
                    printf("\n\t[%d] Paper %d        Score: %6.2f/ %3.0f (%-2s)",
                           n + (n < ignore), n + 1, thisScore, maxMarks,
                           gradeNames[determineGrade(thisScore / maxMarks)]);
                    totalPapers++;
                }
                n++;
            }
            printf("\n\t[9] Edit All\n\t[0] Go Back\n\n");

            ans = -1;
            while (ans < 0 || ans > totalPapers && ans != 9) {
                printf("Select a paper (1-%d): ", totalPapers);
                scanf("%d", &ans);
                clearInput();
            };

            if (ans == 0) {
                continue;
            }

            else if (ans == 9) {
                //  Read multiple integers
                printf("Entering scores for %s\n", currentSubject.name);

                float inputMarks[5];
                for (n = 0; n < 5; n++) {
                    thisScore = subjects[selected].myScore[n];
                    maxMarks = currentSubject.marks[n];
                    if (n == ignore || maxMarks <= 0) {
                        inputMarks[n] = -1.0f;
                        continue;
                    }

                    inputMarks[n] = 999.0f;
                    while ((inputMarks[n] < 0.0f || inputMarks[n] > maxMarks) && inputMarks[n] != -1.0f) {
                        printf("[%d/%d] ", currentSubject.code, n + 1);
                        if (scanf(" %f", &inputMarks[n]) != 1) {
                            printf("Invalid input\n");
                            clearInput();
                        }
                    }
                    clearInput();
                }

                //  Confirm scores
                printf("\n\tPaper\tScore\tScore obtained");

                n = 0;
                while (currentSubject.marks[n] > 0 && n < 5) {
                    if (n != ignore) {
                        thisScore = subjects[selected].myScore[n];
                        maxMarks = currentSubject.marks[n];

                        if (inputMarks[n] == -1) {
                            inputMarks[n] = thisScore;
                        }

                        printf("\n\t%3d/%d\t %3.0f \t%6.2f (%-2s)  ->  ",
                               currentSubject.code, n + 1, maxMarks,
                               thisScore, gradeNames[determineGrade(thisScore / maxMarks)]);
                        if (inputMarks[n] == thisScore) {
                            printf("   Kekal");
                        } else {
                            printf("%s%6.2f (%-2s) %s", CYAN, inputMarks[n],
                               gradeNames[determineGrade(inputMarks[n] / maxMarks)], RESET);
                        }
                    }
                    n++;
                }
                printf("\n\n");

                char yn;
                yn = '\0';
                while (yn != 'N' && yn != 'Y' && yn != 'n' && yn != 'y') {
                    printf("Confirm? (Y/N) ");
                    scanf(" %c", &yn);
                    clearInput();
                }
                if (yn == 'Y' || yn == 'y') {
                    for (n = 0; n < 5; n++) {
                        subjects[selected].myScore[n] = inputMarks[n];
                    }
                } else {
                    continue;
                }
            }

            else {
                int paper = ans - (ans <= ignore);
                maxMarks = currentSubject.marks[paper];

                float inputMark = -0.1;
                while ( (inputMark < 0.0f || inputMark > maxMarks) && inputMark != -1.0f) {
                    printf("Enter score for %s %d (max %.0f) or enter -1 to cancel: ",
                           currentSubject.name, paper + 1, maxMarks);
                    scanf("%f", &inputMark);
                    clearInput();
                }
                if (inputMark == -1) {
                    continue;
                }
                subjects[selected].myScore[paper] = inputMark;
            }

            displayTable();
            printf("\n%sScore updated%s", GREEN, RESET);
        }

        else if (ans == 2) {
            ans = -1;
            for (n = 1; n < numSubjects; n++) {
                printf("\n\t[%d] %s", n, subjectList[subjects[n].subjectIndex].name);
            }
            printf("\n\t[0] Go Back\n\n");

            while (ans < 0 || ans > numSubjects - 1) {
                printf("Select a subject (1-%d): ", numSubjects - 1);
                scanf("%d", &ans);
                clearInput();
            };

            if (ans == 0) {
                continue;   //  Go back
            }
            if (changeSubject(ans) == 0) {
                displayTable();
                printf("%sSubject changed%s", GREEN, RESET);
            }
        }

        else {
            break;
        }
    }

    return 0;
}

//  Helper functions
int getSubjectIndex(int code) {
    // Perform binary search here
    int mid, min = 0, max = totalSubjects - 1;

    while (min <= max) {
        mid = min + (max - min) / 2;

        if (subjectList[mid].code == code) {
            return mid;
        }

        if (subjectList[mid].code < code) {
            min = mid + 1;
        } else {
            max = mid - 1;
        }
    }

    return -1;
}

int subjectExists(int idx, int pos) {
    int n;

    for (n = 0; n < numSubjects; n++) {
        if (n == pos) {
            continue;
        }
        if (idx == subjects[n].subjectIndex) {
            return 1;
        }
    }

    return 0;
}

int changeSubject(int i) {
    if (i == 0) {
        printf("Unable to change subject Pengajian Am");
        return -1;
    }

    char yn;
    int temp_idx, subjectCode, chosen = -1;

    printf("Enter code for ELECTIVE %d: ", i);
    if (scanf("%d", &subjectCode) != 1) {
        clearInput();
        return -1;
    };
    clearInput();
    temp_idx = getSubjectIndex(subjectCode);

    if (temp_idx == -1) {
        printf("%sERROR:%s  Invalid code\n", RED, RESET);
        return -1;
    }

    if (subjectExists(temp_idx, i)) {
        printf("%sERROR:%s  %s is already in the list!\n", RED, RESET, subjectList[temp_idx].name);
        return -1;
    }

    if (subjectList[temp_idx].altPaper[0] > 0) {
        printf("This subject has optional papers:\n\n");
        printf("\t[1] %s %d\n",
               subjectList[temp_idx].name, subjectList[temp_idx].altPaper[0]);
        printf("\t[2] %s %d\n",
               subjectList[temp_idx].name, subjectList[temp_idx].altPaper[1]);
        printf("\n");

        while (chosen < 1 || chosen > 2) {
            printf("Please select a paper (1-2): ");
            if (scanf("%d", &chosen) != 1) {
                clearInput();
                continue;
            };
        }
    }

    printf("\n\tCode   : %d", subjectList[temp_idx].code);
    printf("\n\tSubject: %s", subjectList[temp_idx].name);
    printf("\n\tPapers : ");
    int m;
    for (m = 0; m < 5; m++) {
        if (subjectList[temp_idx].altPaper[(chosen - 1) ^ 1] - 1 == m) {
            continue;
        }
        if (subjectList[temp_idx].marks[m] == 0) {  //  This paper (and probably the following papers) do not exist
            break;
        }
        if (m != 0) {
            printf(",  ");
        }
        printf("%d/%d", subjectList[temp_idx].code, m+1);
    }
    printf("\n\n");

    if (subjects[i].subjectIndex != -1) {
            printf("%sWARNING:%s  Your scores for this entry will be reset!!\n", YELLOW, RESET);
    }
    yn = '\0';
    while (yn != 'N' && yn != 'Y' && yn != 'n' && yn != 'y') {
        printf("Confirm this subject? (Y/N) ");
        scanf(" %c", &yn);
        clearInput();
    }
    if (yn == 'Y' || yn == 'y') {
        subjects[i].subjectIndex = temp_idx;
        for (m = 0; m < 5; m++) {
            subjects[i].myScore[m] = -1;
        }
        subjects[i].chosenOptional = chosen;
        return 0;
    }
    return -1;
}

/* rip old function, u will be missed
const char* determineGrade(float score) {
    if (score < 0 || score > 1) {
        return "?";
    } else if (score >= 0.8f) {
        return "A" ;
    } else if (score < 0.25f) {
        return "F";
    } else {
        int idx = (score * 100.0f - 25) / 5;
        const char *grades[] = {"D", "D+", "C-", "C", "C+", "B-", "B", "B+", "B+", "A-", "A-"};
        return grades[idx];
    }
}
*/

typedef struct {
    float minPercent;
    enum Grade grade;
} GradeThreshold;

enum Grade determineGrade(float percentage) {
    if (percentage > 1.0f) {
        return TIADA;
    }

    static const GradeThreshold grades[] = {
        {0.80f, GRED_A},
        {0.70f, GRED_AM},
        {0.60f, GRED_BP},
        {0.55f, GRED_B},
        {0.50f, GRED_BM},
        {0.45f, GRED_CP},
        {0.40f, GRED_C},
        {0.35f, GRED_CM},
        {0.30f, GRED_DP},
        {0.25f, GRED_D},
        {0.00f, GRED_F}
    };

    int i;
    for (i = 0; i < sizeof(grades) / sizeof(grades[0]); i++) {
        if (percentage >= grades[i].minPercent) {
            return grades[i].grade;
        }
    }
    return TIADA;
};

void displayTable(void) {
    printf("\n---------------------------------------------------------------------------------------------\n");
    printf("|  Code  |                     Subject                      | Weight (%%) |  Rank  | Overall |\n");
    printf("---------------------------------------------------------------------------------------------\n");

    float totalSubjectPoints, totalMax, weight;
    enum Grade grade, totalGrade;
    SubjectInfo currentSubject;

    //  If 5 subjects are taken, gpa will take the highest 4, so we need to keep track of the least gpa and omit it
    float totalGPA = 0.0f, lowestGPA = 4.0f, avgGPA;

    int n, m;
    for (n = 0; n < numSubjects; n++) {
        //  Calculate total score obtained and max score
        currentSubject = subjectList[subjects[n].subjectIndex];
        totalSubjectPoints = totalMax = 0;

        //  Determine total points of the subject
        for (m = 0; m < 5; m++) {
            if (currentSubject.altPaper[(subjects[n].chosenOptional - 1) ^ 1] - 1 == m) {
                continue;
            }
            if (currentSubject.marks <= 0) {
                break;
            }
            if (subjects[n].myScore[m] >= 0) {
                totalSubjectPoints += subjects[n].myScore[m];
            }
            totalMax += currentSubject.marks[m];
        }
        totalGrade = determineGrade(totalSubjectPoints / totalMax);
        totalGPA += gpa[totalGrade];
        if (gpa[totalGrade] < lowestGPA) {
            lowestGPA = gpa[totalGrade];
        }

        //  Header
        printf("|  %4d  | %-46s   |            |        |    %-2s   |\n",
               currentSubject.code, currentSubject.name, gradeNames[totalGrade]);

        //  Print individual papers
        for (m = 0; m < 5; m++) {
            if (currentSubject.altPaper[(subjects[n].chosenOptional - 1) ^ 1] - 1 == m) {
                continue;
            }
            if (currentSubject.marks[m] == 0) {  //  This paper (and probably the following papers) do not exist
                break;
            }
            grade = determineGrade(subjects[n].myScore[m] / currentSubject.marks[m]);
            weight = 100 * currentSubject.marks[m] / totalMax;
            printf("|        |     %-42s %d |    %5.2f   |   %-2s   |",
                   currentSubject.name, m + 1, weight, gradeNames[grade]);
            if (m == 0) {
                printf("   %4.2f  |\n", gpa[totalGrade]);
            } else {
                printf("         |\n");
            }
        }

        printf("---------------------------------------------------------------------------------------------\n");
    }

    if (numSubjects > 4) {
        totalGPA -= lowestGPA;
    }
    avgGPA = totalGPA / 4;
    printf("Overall CGPA:   %.2f", avgGPA);
}

void clearInput() {
    while (getchar() != '\n');
}
