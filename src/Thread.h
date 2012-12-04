// TrafficWatcher - a network speed monitor

// Copyright (C) 2008 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

/**
 * CThread.
 * a simple wrapper class for implementing a thread in a object oriented way.
 * just inherit your own class from CThread and implement the virtual method.
 *
 * Example:
 * \code
 * class CMyThreadClass : CThread
 * {
 *      ...
 *      DWORD   ThreadMethod();
 *      ...
 * };
 * CMyThreadClass::ThreadMethod()
 * {
 *      //place here your code that runs as an own thread
 *      while (m_runthread)
 *      {
 *          printf("my ThreadMethod is running");
 *          Sleep(1000);
 *      }
 * }
 *
 * in another class (or in another method of your CMyThreadClass):
 * ...
 * CMyThreadClass m_mythread;
 * ...
 * m_mythread.Start();
 * ...
 * m_mythread.Stop();
 * ...
 * //to ensure that the thread was stopped
 * if (m_mythread.Stop() == STILL_ACTIVE)
 *      m_mythread.Stop(true);          //this kills the thread
 * \endcode
 * instead of the last lines of code you can use for short the following:
 * \code
 * m_mythread.Start();
 * ...
 * m_mythread.Stop(1000);       //give the thread 1 second to stop itself
 * //now the thread either stopped itself or was killed after one second automatically.
 * \endcode
 */

class CThread
{
public:
        /**
         * Default Constructor
         */
        CThread()
        {
            m_pThreadFunction = CThread::EntryPoint;
        }

        /**
         * Default Destructor
         * also destroys the thread
         */
        ~CThread()
        {
            if ( m_hThread )
                Stop(true);                 //thread still running, so force the thread to stop!
        }
        /**
         * Starts the thread.
         * \param dwCreationFlags       the flags to use for creating the thread. see CreateThread() in the windows sdk.
         */
        DWORD Start(DWORD dwCreationFlags = 0)
        {
            m_runthread = true;
            m_hThread = CreateThread(NULL, 0, m_pThreadFunction, this, 0, &dwCreationFlags);
            m_dwExitCode = (DWORD)-1;

            return GetLastError();
        }

        /**
         * Stops the thread.
         *
         * \param bForceKill        if true, the Thread is killed immediately
         */
        DWORD Stop ( bool bForceKill = false )
        {
            if ( m_hThread )
            {
                if (m_runthread == TRUE)
                    m_runthread = FALSE;        //first, try to stop the thread nice

                GetExitCodeThread(m_hThread, &m_dwExitCode);

                if ( m_dwExitCode == STILL_ACTIVE && bForceKill )
                {
                    TerminateThread(m_hThread, DWORD(-1));
                    m_hThread = NULL;
                }
            }

            return m_dwExitCode;
        }
        /**
         * Stops the thread. first tell the thread to stop itself and wait for the thread to stop itself.
         * if timeout occurs and the thread hasn't stopped yet, then the thread is killed.
         * \param timeout   milliseconds to wait for the thread to stop itself
         */
        DWORD Stop ( WORD timeout )
        {
            Stop(false);
            WaitForSingleObject(m_hThread, timeout);
            return Stop(true);
        }

        /**
         * suspends the thread. i.e. the thread is halted but not killed. To start a suspended thread call Resume().
         */
        DWORD Suspend()
        {
            return SuspendThread(m_hThread);
        }

        /**
         * resumes the thread. this method starts a created and suspended thread again.
         */
        DWORD Resume()
        {
            return ResumeThread(m_hThread);
        }

        /**
         * sets the priority of the thread.
         * \param priority  the priority. see SetThreadPriority() in windows sdk for possible values.
         * \return true if successful
         */
        BOOL SetPriority(int priority)
        {
            return SetThreadPriority(m_hThread, priority);
        }

        /**
         * gets the current priority value of the thread.
         * \return the current priority value
         */
        int GetPriority()
        {
            return GetThreadPriority(m_hThread);
        }

protected:

        /**
         * Override this method.
         *
         * this method works as the thread!
         */
        virtual DWORD ThreadMethod() = 0;


private:

        /**
         * DONT override this method.
         *
         * this method is the "function" used when creating the thread. it is static so that way
         * a pointer to it is available inside the class. this method calls then the virtual
         * method of the parent class.
         */
        static DWORD WINAPI EntryPoint( LPVOID pArg)
        {
            CThread *pParent = reinterpret_cast<CThread*>(pArg);

            pParent->ThreadMethod();

            return 0;
        }


private:
    HANDLE  m_hThread;                  ///<Thread Handle
    DWORD   m_dwTID;                    ///<Thread ID
    LPVOID  m_pParent;                  ///<this pointer of the parent CThread object
    DWORD   m_dwExitCode;               ///<Exit Code of the thread

protected:
    LPTHREAD_START_ROUTINE  m_pThreadFunction;  ///<The Worker Thread Function Pointer
    BOOL    m_runthread;                ///<indicates if thread should be running
};
