#include <stdio.h>
#include <math.h>
#define EPSILON 0.001
#define N 5   // Maximum number of points
#define X 20  // Percentage for classifying points into clusters


int main(){
    // Variables for coordinates and cluster counts
    double x0, x1, x2, x3;
    double y0, y1, y2, y3;
    int n1, n2, n3;
    int counter = 0;
    double dis1,dis2,dis3;
    n1=n2=n3= 1;

    // Input for initial cluster coordinates
    printf("Cluster 1 coordinates: ");
    scanf("%lf %lf", &x1, &y1);
    printf("Cluster 2 coordinates: ");
    scanf("%lf %lf", &x2, &y2);
    printf("Cluster 3 coordinates: ");
    scanf("%lf %lf", &x3, &y3);
    printf("%.2f %.2f %.2f %.2f %.2f %.2f\n", x1, y1, x2, y2, x3, y3);

    while(counter != N){

        printf("\nEnter new point: ");
        scanf("%lf %lf", &x0, &y0);

        // Calculate distances from the new point to each cluster
        dis1=sqrt(pow(x0-x1,2)+pow(y0-y1,2));
        dis2=sqrt(pow(x0-x2,2)+pow(y0-y2,2));
        dis3=sqrt(pow(x0-x3,2)+pow(y0-y3,2));

        double closest, second_closest;
        int closest_cluster, second_closest_cluster;

        // Determine the closest and second closest clusters
        if (dis1 <= dis2 && dis1 <= dis3) {
            closest = dis1;
            closest_cluster = 1;
            if (dis2 <= dis3) {
                second_closest = dis2;
                second_closest_cluster = 2;
            } else {
                second_closest = dis3;
                second_closest_cluster = 3;
            }
        } else if (dis2 <= dis1 && dis2 <= dis3) {
            closest = dis2;
            closest_cluster = 2;
            if (dis1 <= dis3) {
                second_closest = dis1;
                second_closest_cluster = 1;
            } else {
                second_closest = dis3;
                second_closest_cluster = 3;
            }
        } else {
            closest = dis3;
            closest_cluster = 3;
            if (dis1 <= dis2) {
                second_closest = dis1;
                second_closest_cluster = 1;
            } else {
                second_closest = dis2;
                second_closest_cluster = 2;
            }
        }

        if (closest == 0) {
            closest += EPSILON;
        }
        
        if (second_closest == 0) {
            second_closest += EPSILON;
        }


        if (((second_closest - closest) / closest * 100) >= X) {
            printf("Point %.2f, %.2f was included in Cluster %d.\n", x0, y0, closest_cluster);
            
            if (closest_cluster == 1) {
                x1 = (x1 * n1 + x0) / (n1 + 1);
                y1 = (y1 * n1 + y0) / (n1 + 1);
                n1++;
            } else if (closest_cluster == 2) {
                x2 = (x2 * n2 + x0) / (n2 + 1);
                y2 = (y2 * n2 + y0) / (n2 + 1);
                n2++;
            } else if (closest_cluster == 3) {
                x3 = (x3 * n3 + x0) / (n3 + 1);
                y3 = (y3 * n3 + y0) / (n3 + 1);
                n3++;
            }
        }else {
            printf("Point %.2f, %.2f was discarded.\n", x0, y0);
        }

        counter++;// Increment the number of points

    }

    printf("Cluster 1: %f, %f, %d\n", x1, y1, n1);
    printf("Cluster 2: %f, %f, %d\n", x2, y2, n2);
    printf("Cluster 3: %f, %f, %d\n", x3, y3, n3);


    return 0;
}

