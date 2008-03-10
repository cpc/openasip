/*****************************************************************************
 * This is a simple C program to demonstrate TCE and usage of user defined
 * custom operations.
 *
 ****************************************************************************/

/* Global variables just to be able to access them easily in ttasim as it 
   imports only global symbols at the moment. */
int sum1;
int sum2;

int value_vector[] = {1, 2, 3, 4, 5, 6, 7, 8};

int main() {
    int i, v1, v2, v3, v4;

    for( i = 0; i < 100; i++){
        v1 = value_vector[0];
        v2 = value_vector[1];
        v3 = value_vector[2];
        v4 = value_vector[3];
        sum1 = v1 + v2 + v3 + v4;

        v1 = value_vector[4];
        v2 = value_vector[5];
        v3 = value_vector[6];
        v4 = value_vector[7];
        sum2 = v1 + v2 + v3 + v4;
    }

    return 0;
}
