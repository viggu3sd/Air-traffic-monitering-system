#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include<stdbool.h>
#include <math.h>

#define MAX 100005 

typedef struct Plane_type
{
    int id; 
    int time;
    bool action;
    bool emergency;
    
}Plane_type;

void swap(Plane_type *a, Plane_type *b)
{
   Plane_type temp = *a;
   *a = *b;
   *b = temp;
}


Plane_type arrivalQueue[MAX];
Plane_type departureQueue[MAX];
Plane_type emergencyLandingQueue[MAX];


bool isEmpty(Plane_type pq[])
{
    return pq[0].id==0;
}

bool isSmaller(Plane_type a, Plane_type b)
{
    if(a.time == b.time)
    {
        if( a.emergency == b.emergency)
        {
            return a.id < b.id;
        }
        return a.emergency > b.emergency;
    }

    return a.time < b.time;
}

void min_heapify(int i, int n, Plane_type pq[])
{
   int l = 2 * i;
   int r = 2 * i + 1;
   int smaller = i;
   if (l <= n && isSmaller(pq[l],pq[smaller]))
      smaller = l;
   if (r <= n && isSmaller(pq[r],pq[smaller]))
      smaller = r;
   if (smaller != i)
   {
      swap(&pq[i], &pq[smaller]);
      min_heapify(smaller, n , pq);
   }
}

void mnheapify(int n, Plane_type pq[])
{
   int parent = n / 2;
   if (parent > 0)
   {
      if (isSmaller(pq[n],pq[parent]))
      {
         swap(&pq[parent], &pq[n]);
         mnheapify(parent , pq);
      }
   }
}

void enQueuePlane(Plane_type newPlane, Plane_type pq[])
{
    int n = pq[0].id;
    n = n + 1;
    pq[n] = newPlane;
    pq[0].id = n;
    mnheapify(n, pq);
}

void deQueuePlane(Plane_type pq[])
{
    int n = pq[0].id;
    pq[1] = pq[n];
    pq[0].id--;
    min_heapify(1, pq[0].id, pq);
}

Plane_type nextPlane(Plane_type pq[])
{
    return pq[1];
}


void startSimulation(int *totalPlanes,int *totalRunways)
{
    scanf("%d", totalRunways);
    scanf("%d", totalPlanes);
    Plane_type planes[*totalPlanes];

    for (int i = 0; i < *totalPlanes; i++)
    {
        Plane_type plane;
        int emergency,action;
        scanf("%d %d %d %d", &plane.id, &plane.time, &action, &emergency);
        plane.action = action;
        plane.emergency = emergency;
        planes[i] = plane;
    }

    for (int i = 0; i < *totalPlanes; i++)
    {
        Plane_type newPlane = planes[i];
        
        if (newPlane.action == 0)
        {
            
            if(newPlane.emergency)
            {
                enQueuePlane(newPlane, emergencyLandingQueue);
            }
            else
            {
                enQueuePlane(newPlane, arrivalQueue);
            }
        }
        else
        {
            enQueuePlane(newPlane, departureQueue);
        }
    }
}

int calculateMinTime(Plane_type q1[], Plane_type q2[])
{
    int temp=1e9;
    if(!isEmpty(q1))
    {
        temp=nextPlane(q1).time;
    }
    if(!isEmpty(q2))
    {
        if(nextPlane(q2).time < temp)
        {
            temp=nextPlane(q2).time;
        }
    }

    return temp;
}

void takeOff(Plane_type q[],int currentTime, int* currentFreeRunway, int* departingPlanes, int* waitingTimeForDeparture,int* largestWaitTimeForDeparture)
{
    Plane_type plane = nextPlane(q);
    deQueuePlane(q);
    int delay = currentTime - plane.time;
    *waitingTimeForDeparture+=delay;
    (*departingPlanes)++;
    (*currentFreeRunway)++;
    if(currentTime-plane.time > *largestWaitTimeForDeparture)
    {
        *largestWaitTimeForDeparture=currentTime-plane.time;
    }

    printf("Plane %d took off at time %d from runway %d with a delay of %d units.\n", plane.id, currentTime, *currentFreeRunway, delay);
}

void land(Plane_type q[],int currentTime, int* currentFreeRunway, int* arrivingPlanes, int* waitingTimeForArrival,int*largestWaitTimeForArrival)
{
    Plane_type plane = nextPlane(q);
    deQueuePlane(q);
    int delay = currentTime - plane.time;
    *waitingTimeForArrival+=delay;
    (*arrivingPlanes)++;
    (*currentFreeRunway)++;
    if(currentTime-plane.time > *largestWaitTimeForArrival)
    {
        *largestWaitTimeForArrival=currentTime-plane.time;
    }

    if(plane.emergency)
    {
        printf("Plane %d landed at time %d on runway %d with a delay of %d units due to emergency.\n", plane.id, currentTime, *currentFreeRunway, delay);
        return;
    }
    else
    {
        printf("Plane %d landed at time %d on runway %d with a delay of %d units.\n", plane.id, currentTime, *currentFreeRunway, delay);
    }
}


void runSimulation(int*totalPlanes,int* arrivingPlanes,int *departingPlanes,int*totalRunways,int*totalTime, float* avgWaitTimeForArrival,float* avgWaitTimeForDeparture, int* largestWaitTimeForArrival,int* largestWaitTimeForDeparture, int* idleTime)
{ 
    
    int minStartTime1 = calculateMinTime(arrivalQueue, departureQueue);
    int minStartTime2 = calculateMinTime(emergencyLandingQueue, departureQueue);
    int currentTime = minStartTime1 < minStartTime2 ? minStartTime1 : minStartTime2;
    
    int currentFreeRunway = 0;
    
    int waitingTimeForArrival = 0;
    int waitingTimeForDeparture = 0;

    while (!isEmpty(arrivalQueue) || !isEmpty(departureQueue) || !isEmpty(emergencyLandingQueue))
    {
        bool emergencyLanding = false;
        if(!isEmpty(emergencyLandingQueue))
        {
            Plane_type arrivalPlane = nextPlane(emergencyLandingQueue);
            if(arrivalPlane.time <= currentTime)
            {
                emergencyLanding = true;
                land(emergencyLandingQueue, currentTime, &currentFreeRunway, arrivingPlanes, &waitingTimeForArrival, largestWaitTimeForArrival);
            }
        }
        
        if(emergencyLanding == false)
        {
            if (isEmpty(arrivalQueue))
            {
                takeOff(departureQueue,currentTime, &currentFreeRunway, departingPlanes, &waitingTimeForDeparture, largestWaitTimeForDeparture);
            }
            else if (isEmpty(departureQueue))
            {
                land(arrivalQueue,currentTime, &currentFreeRunway, arrivingPlanes, &waitingTimeForArrival, largestWaitTimeForArrival);
            }
            else
            {
                Plane_type arrivalPlane = nextPlane(arrivalQueue);
                Plane_type departurePlane = nextPlane(departureQueue);
                
                if(arrivalPlane.time <= currentTime || departurePlane.time <= currentTime)
                {
                    if(arrivalPlane.time > currentTime)
                    {
                        takeOff(departureQueue,currentTime, &currentFreeRunway, departingPlanes, &waitingTimeForDeparture, largestWaitTimeForDeparture);
                    }
                    else if(departurePlane.time > currentTime)
                    {
                        land(arrivalQueue,currentTime, &currentFreeRunway, arrivingPlanes, &waitingTimeForArrival, largestWaitTimeForArrival);
                    }
                    else
                    {
                        int takeOffDelay = currentTime-departurePlane.time;
                        int landDelay = currentTime-arrivalPlane.time;
                        if(takeOffDelay <= 2*landDelay)
                        {
                            land(arrivalQueue,currentTime, &currentFreeRunway, arrivingPlanes, &waitingTimeForArrival, largestWaitTimeForArrival);
                        }
                        else
                        {
                            takeOff(departureQueue,currentTime, &currentFreeRunway, departingPlanes, &waitingTimeForDeparture, largestWaitTimeForDeparture);
                        }
                    }
                }
            }
        }

        

        if(isEmpty(arrivalQueue) && isEmpty(departureQueue))
        {
            break;
        }

        minStartTime1 = calculateMinTime(arrivalQueue, departureQueue);
        minStartTime2 = calculateMinTime(emergencyLandingQueue, departureQueue);
        int calculatedMinTime = minStartTime1 < minStartTime2 ? minStartTime1 : minStartTime2;
        
        if(calculatedMinTime > currentTime)
        {
            *idleTime+=(calculatedMinTime-currentTime-1);
            currentTime=calculatedMinTime;
            currentFreeRunway=0;
        }

        if(currentFreeRunway == *totalRunways)
        {
            currentFreeRunway=0;
            currentTime++;
        }
    }

    float a = 1.0*waitingTimeForArrival;
    float b = 1.0*(*arrivingPlanes);
    *avgWaitTimeForArrival = b>0 ? a/b : 0;

    float c = 1.0*waitingTimeForDeparture;
    float d = 1.0*(*departingPlanes);
    *avgWaitTimeForDeparture = d>0? c/d : 0;

    *totalTime=currentTime;
}

void endSimulation(int totalPlanes,int arrivingPlanes,int departingPlanes, int totalRunways, int totalTime, float avgWaitTimeForArrival, float avgWaitTimeForDeparture, int largestWaitTimeForArrival, int largestWaitTimeForDeparture,int idleTime)
{
    printf("\nTotal planes: %d\n", totalPlanes);
    printf("Total arriving planes: %d\n", arrivingPlanes);
    printf("Total departing planes: %d\n", departingPlanes);
    printf("Total runways: %d\n", totalRunways);
    printf("Total time: %d\n", totalTime);
    printf("Average wait time for arrival: %f\n", avgWaitTimeForArrival);
    printf("Average wait time for departure: %f\n", avgWaitTimeForDeparture);
    printf("Largest wait time for arrival: %d\n", largestWaitTimeForArrival);
    printf("Largest wait time for departure: %d\n", largestWaitTimeForDeparture);
    printf("Idle time: %d\n", idleTime);
}

int main()
{
    arrivalQueue[0].id=0;
    departureQueue[0].id=0;

    int totalPlanes, arrivingPlanes,departingPlanes,totalRunways,totalTime, idleTime,largestWaitTimeForArrival,largestWaitTimeForDeparture;
    float avgWaitTimeForArrival, avgWaitTimeForDeparture;
    startSimulation(&totalPlanes,&totalRunways);
    runSimulation(&totalPlanes, &arrivingPlanes, &departingPlanes, &totalRunways, &totalTime, &avgWaitTimeForArrival, &avgWaitTimeForDeparture, &largestWaitTimeForArrival, &largestWaitTimeForDeparture, &idleTime);
    endSimulation(totalPlanes,arrivingPlanes,departingPlanes, totalRunways, totalTime, avgWaitTimeForArrival, avgWaitTimeForDeparture, largestWaitTimeForArrival, largestWaitTimeForDeparture, idleTime);
    return 0;
}