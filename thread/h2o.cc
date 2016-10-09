// ****************************************************************************
// This is a sample multi-threaded program to illustrate the use of mutexes
// and semaphores in pthreads.  As you know, the water molecule is made up of
// 2 H atoms and one 0 atom.  In this program, we spawn off a bunch of H and
// O atoms.  The atom sleeps a random amount of time.  When it wakes up, if
// there is enough of the other atoms to make water, water is made.  This is
// evaluated by looking at the values of some global state variables.  Then
// semaphores are used to signal that atoms can terminate since they've been
// involved in making up a water molecule.
// ****************************************************************************

#include	<stdlib.h>
#include <unistd.h>
#include	<iostream.h>
#include	<pthread.h>
#include	<semaphore.h>

int total = 0; // The total number of atoms to create (will be a multiple of 3)

// The mutex (i.e., binary semaphore used to protect access to the state 
// variables below.  Note that a statically allocated mutex (such as this)
// does not need to be explicitly initialized.  By default it is all zeros.

pthread_mutex_t my_lock;

// Two global state variables:

int h_atoms; // The number of H atoms
int o_atoms; // The number of O atoms

// Semaphores used in releasing H and O atoms, i.e., allowing the threads
// to terminate.

sem_t release_h;
sem_t release_o;

// The function a hydrogen atom executes.

void *h_atom( void * )
{
	cout << "One H atom" << endl;
	sleep( rand() % 5 ); // Sleep a random amount of time.

	// Before changing the state, protect the change by a pthread_mutex_lock
	// and pthread_mutex_unlock.

	pthread_mutex_lock( &my_lock );
	h_atoms++; // One more hydrogen atom has been created

	if( h_atoms >= 2 && o_atoms >= 1 )
	{
		// There are enough H and O atoms to make water.

		cout << "H2O: water is made!" << endl;

		// Two H and one O atom is involved in making water.  Decrease the
		// state variables to indicate these atoms have been "used up".

		h_atoms -= 2;
		o_atoms--;

		// Now signal two H and one O atom that they can terminate, since
		// they have been involved in making water.  Note that it is ok to
		// signal from inside a critical section since signalling is non-blocking.

		sem_post( &release_h );
		sem_post( &release_h );
		sem_post( &release_o );
	}

	pthread_mutex_unlock( &my_lock );

	// This is an H atom and so it needs to wait until it has been used in
	// a chemical reaction of making water.  Once it has, it can terminate.

	sem_wait( &release_h );
}

// The function an oxygen atom executes.

void *o_atom( void * )
{
	// This function is virtually identical to the H atom function.  Only
	// differences will be highlighted.

	cout << "One O atom" << endl;
	sleep( rand() % 5 );

	pthread_mutex_lock( &my_lock );
	o_atoms++; // This is an O atom, so the state changes to indicate this.

	if( h_atoms >= 2 && o_atoms >= 1 )
	{
		cout << "H2O: water is made!" << endl;
		h_atoms -= 2;
		o_atoms--;
		sem_post( &release_h );
		sem_post( &release_h );
		sem_post( &release_o );
	}

	pthread_mutex_unlock( &my_lock );

	// The O atom waits on a different semaphore, but still waits to be
	// involved in the making of water.

	sem_wait( &release_o );
}

int main()
{
	int i;
	pthread_t *tid;
	unsigned int seed;

	// A seed value is required for the pseudo-random number generator
	// (since otherwise the same sequence of values will always be generated).

	cout << "Enter a seed value (an int): ";
	cin >> seed;
	cout << endl;

	srand( seed ); // Set the seed for the pseudo-random number generator

	// Get a random number of atom to create that is a multiple of 3.

	do{
		total = rand()% 50 + 1;
	}while( total % 3 != 0 );

	cout << "Main: a total of " << total;
	cout << " H and O atoms will be created." << endl;

	// Allocate an array to hold enough thread identifiers for as many
	// atoms as we will create.

	tid = new (pthread_t)[ total ];

	// Initially, there are no H or O atoms in the system.  As well, the
	// semaphores are initialized to zero.

	h_atoms = o_atoms = 0;
	sem_init( &release_h, 0, 0 );
	sem_init( &release_o, 0, 0 );

	// Create twice as many H as O atoms.

	for( i = 0; i < total; i++ )
		if( i % 3 != 0 )
			pthread_create( &tid[ i ], NULL, h_atom, NULL );
		else
			pthread_create( &tid[ i ], NULL, o_atom, NULL );

	// Wait for all the atoms to terminate, then the main thread will terminate.

	for( i = 0; i < total; i++ )
		pthread_join( tid[ i ], NULL );

	return 0;
}
