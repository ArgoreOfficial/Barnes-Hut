#include "cThreadPool.h"

#include "sPoint.h"
#include "cOctree.h"
#include "cNode.h"

#include <chrono>
#include <Windows.h>

cThreadPool::cThreadPool()
{

}

cThreadPool::~cThreadPool()
{

}

#define BIG_G 6.67e-11
static void doThreadWork( cThreadPool::sWorker* _worker )
{
	while ( _worker->running )
	{
		if ( !_worker->busy )
		{
			Sleep( 1 );
			continue;
		}

		while ( _worker->point_queue.size() > 0 )
		{
			_worker->mutex.lock();
			sPoint* point = _worker->point_queue.front();

			wv::cVector3d f;

			f -= _worker->octree->computeForces( *point );
			point->velocity += ( f * BIG_G ) / point->mass * _worker->delta_time;
			_worker->point_queue.erase( _worker->point_queue.begin() );
			_worker->mutex.unlock();
		}

		_worker->mutex.lock();
		_worker->busy = false;
		_worker->thread_pool->threadFinished();
		_worker->mutex.unlock();
	}
}

void cThreadPool::createWorkers( unsigned int _count, cOctree* _octree )
{
	for ( int i = 0; i < _count; i++ )
	{
		sWorker* worker = new sWorker();
		worker->running = true;
		worker->octree = _octree;

		worker->thread = std::thread( doThreadWork, worker );
		worker->thread_pool = this;
		m_workers.push_back( worker );
	}
}

void cThreadPool::queueWork( sPoint* _point, double _delta_time )
{
	sWorker& worker = *m_workers[ m_last_worked ];

	worker.mutex.lock();
	worker.point_queue.push_back( _point );
	worker.delta_time = _delta_time;
	
	if ( !worker.busy )
	{
		m_mutex.lock();
		m_busy_workers++;
		m_mutex.unlock();
	}

	worker.busy = true;
	worker.mutex.unlock();
	
	m_last_worked++;
	m_last_worked %= m_workers.size();
}

void cThreadPool::threadFinished()
{
	m_mutex.lock();
	m_busy_workers--;
	m_mutex.unlock();
}

