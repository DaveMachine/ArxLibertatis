/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform/Lock.h"

#include "platform/Platform.h"

#if defined(HAVE_PTHREADS)

Lock::Lock() : locked(false) {
	const pthread_mutex_t mutex_init = PTHREAD_MUTEX_INITIALIZER;
	mutex = mutex_init;
	const pthread_cond_t cond_init = PTHREAD_COND_INITIALIZER;
	cond = cond_init;
}

Lock::~Lock() {
	
}

void Lock::lock() {
	
	pthread_mutex_lock(&mutex);
	
	while(locked) {
		int rc = pthread_cond_wait(&cond, &mutex);
		arx_assert(rc == 0);
		ARX_UNUSED(rc);
	}
	
	locked = true;
	pthread_mutex_unlock(&mutex);
}

void Lock::unlock() {
	pthread_mutex_lock(&mutex);
	locked = false;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

#elif defined(HAVE_WINAPI)

Lock::Lock() {
	mutex = CreateMutex(NULL, false, NULL);
}

Lock::~Lock() {
	unlock();
	CloseHandle(mutex);
}

void Lock::lock() {
	DWORD rc = WaitForSingleObject(mutex, INFINITE);
	arx_assert(rc == WAIT_OBJECT_0);
	ARX_UNUSED(rc);
}

void Lock::unlock() {
	ReleaseMutex(mutex);
}

#endif
