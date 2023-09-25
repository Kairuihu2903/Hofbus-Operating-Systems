#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
//pthread_mutex_init, 
//pthread_cond_init, 
//pthread_mutex_lock, 
//pthread_mutex_unlock, 
//pthread_cond_wait, 
//pthread_cond_signal, 
//pthread_cond_broadcast

struct station {
	/*
	int waiting_students; // the number of waiting students
	int free_seats; // the number of free seats
	int standing_students; //the turn of the next student to board
	struct condition *cond_hofbus_arrived;
	struct condition *cond_all_students_seated;
	struct lock *lock;
	*/
	////////////////////////////

	
	int free_seats; // number of free seats in the bus, -if one exists at station-.
    int passengers_waiting; // number of passengers waiting at station
	int passengers_leaving; // number of people on the bus
	int turn;			// turn of the next next student to go
	pthread_mutex_t mutex;
    pthread_cond_t free_seats_available; // train arrived with free seats
    pthread_cond_t passengers_on_board; // all leaving passengers have been seated

};

void station_init(struct station *station)
{
	/*
	station->cond_hofbus_arrived = malloc(sizeof(struct condition));
	station->cond_all_students_seated = malloc(sizeof(struct condition));
	station->lock = malloc(sizeof(struct lock));

	cond_init(station->cond_hofbus_arrived);
	cond_init(station->cond_all_students_seated);
	lock_init(station->lock);

	station->standing_students = 0;
	station->free_seats = 0;
	station->waiting_students = 0;
	*/
	

	//////////////////////
	station->free_seats = 0;
    station->passengers_waiting = 0;
    station->passengers_leaving = 0;
	station->turn = 0;

	pthread_mutex_init(&(station->mutex), NULL);
    pthread_cond_init(&(station->free_seats_available), NULL);
    pthread_cond_init(&(station->passengers_on_board), NULL);

}

void station_load_bus(struct station *station, int count)
{
	/*
	lock_acquire(station->lock);
	station->free_seats = count;
	printf("hofbus arrives (count: %d)->", count); 
	print_station(station);

	while ((station->waiting_students > 0) && (station->free_seats > 0)) 
	{
		cond_broadcast(station->cond_hofbus_arrived, station->lock);
		cond_wait(station->cond_all_students_seated, station->lock);
	}

	station->free_seats = 0;
	lock_release(station->lock);
*/
  	pthread_mutex_lock(&(station->mutex)); // enter critical section

    if (count == 0 || station->passengers_waiting == 0) { // no free seats available or no passengers waiting
        pthread_mutex_unlock(&(station->mutex)); // release the lock and leave critical section
		
        return; // train must leave the station promptly
    }

		//printf("hofbus arrived (count: %d)-> \n", count); 
		station->free_seats = count;
		
		pthread_cond_broadcast(&(station->free_seats_available)); // wake up all passengers waiting for a free seat

		while(station->free_seats != 0 && station->passengers_waiting !=  0)
		{
			pthread_cond_broadcast(&(station->passengers_on_board)); // wake up all passengers waiting for a free seat
			pthread_cond_wait(&(station->passengers_on_board), &(station->mutex)); // waiting for all passengers to get on board
		}

		station->free_seats = 0;
		pthread_mutex_unlock(&(station->mutex));
		
		return;
		//pthread_mutex_unlock(&(station->mutex)); // leave critical section
		//return;



}

int station_wait_for_bus(struct station *station, int myticket, int myid)
{


	/*
	lock_acquire(station->lock);
	station->standing_students--;
	station->free_seats--;
	if ((station->free_seats == 0) || (station->standing_students == 0))
		cond_signal(station->cond_all_students_seated, station->lock);
	lock_release(station->lock);
	return myid;
	*/




	//wait for turn
	//keep track if its your turn
	//if it is then the student will go in
	//make a for loop to check if the existing turn in the station is equal to tickets
	//if the student number is a valid value 
	//if it is then you can sleep the number of students still standing 
	//the ticket value is an absolute value 
	//
	//
	//
	//
	// compare if its your turn.
	// if yes increase ticket id and wakeup next student. 
	// dont need to decrease ticket value.
	// 

	// if the student gets on the bus then you will retun the student myid;

	pthread_mutex_lock(&(station->mutex)); // enter critical section
// leave critical section

    station->passengers_waiting++;

	// while loop to make sure that the everyone gets their tickets before bourding
    while (!station->free_seats)
	{
        pthread_cond_wait(&(station->free_seats_available), &(station->mutex)); // waiting for a train with free seats
	}
	while(station->free_seats == 0 || myticket != station->turn+1)
	{	
		//printf("(turn: %d)-> \n", station->turn+1); 
		//printf("(ticket: %d)-> \n", myticket); 

		pthread_cond_wait(&(station->passengers_on_board), &(station->mutex)); // waiting for all passengers to get on board
		
	}

	if(station->free_seats != 0 || station->passengers_waiting == 0 )
	{
		station->turn++;
    	station->passengers_waiting--; 
    	station->passengers_leaving++;
    	station->free_seats--;
    	pthread_mutex_unlock(&(station->mutex)); // leave critical section
	}

		//printf("(turn: %d)-> \n", station->turn);
		//printf("(pwait: %d)-> \n", station->passengers_waiting);
		//printf("(pleave: %d)-> \n", station->passengers_leaving);
		//printf("(freeseats: %d)-> \n", station->free_seats);
		
		pthread_cond_signal(&(station->passengers_on_board));
		// make sure that everyone is on the bus before bus leaves


	return myticket;
	
}


