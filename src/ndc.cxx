// Module:  Log4CPLUS
// File:    ndc.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//
// $Log: not supported by cvs2svn $
// Revision 1.4  2003/04/18 22:08:19  tcsmith
// Converted from std::string to log4cplus::tstring.
//
// Revision 1.3  2003/04/03 01:06:41  tcsmith
// Standardized the formatting.
//

#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/pointer.h>
#include <exception>
#include <utility>
#include <vector>

using namespace log4cplus;
using namespace log4cplus::helpers;


///////////////////////////////////////////////////////////////////////////////
// File Local definitions
///////////////////////////////////////////////////////////////////////////////

namespace {
    class _static_NDC_initializer {
    public:
        _static_NDC_initializer() { getNDC(); }
    } initializer;
}



///////////////////////////////////////////////////////////////////////////////
// public methods
///////////////////////////////////////////////////////////////////////////////

NDC& 
log4cplus::getNDC()
{
    static NDC singleton;
    return singleton;
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::DiagnosticContext ctors
///////////////////////////////////////////////////////////////////////////////

DiagnosticContext::DiagnosticContext(const log4cplus::tstring& message, DiagnosticContext* parent)
 : message(message),
   fullMessage( (parent == NULL) ? message : 
                                   parent->fullMessage + LOG4CPLUS_TEXT(" ") + message )

{
}


DiagnosticContext::DiagnosticContext(const log4cplus::tstring& message)
 : message(message),
   fullMessage(message)
{
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::NDC ctor and dtor
///////////////////////////////////////////////////////////////////////////////

NDC::NDC() 
 : threadLocal(LOG4CPLUS_THREAD_LOCAL_INIT)
{
}


NDC::~NDC() 
{
    LOG4CPLUS_THREAD_LOCAL_CLEANUP( threadLocal );
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::NDC public methods
///////////////////////////////////////////////////////////////////////////////

void
NDC::clear()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            delete ptr;
            LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::clear()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::clear()- exception occured"));
    }
}


DiagnosticContextStack
NDC::cloneStack()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            return DiagnosticContextStack(*ptr);
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::cloneStack()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::cloneStack()- exception occured"));
    }

    return DiagnosticContextStack();
}


void 
NDC::inherit(const DiagnosticContextStack& stack)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            delete ptr;
        }

        ptr = new DiagnosticContextStack(stack);
        LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, ptr );
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::inherit()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::inherit()- exception occured"));
    }
}


log4cplus::tstring 
NDC::get()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            return ptr->top().fullMessage;
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::get()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::get()- exception occured"));
    }

    return LOG4CPLUS_TEXT("");
}


int 
NDC::getDepth()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            return ptr->size();
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::getDepth()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::getDepth()- exception occured"));
    }

    return 0;
}


log4cplus::tstring 
NDC::pop()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            DiagnosticContext dc = ptr->top();
            ptr->pop();
            if(ptr->empty()) {
                // If the NDC stack is empty we will delete it so that we can avoid
                // most memory leaks if Threads don't call remove when exiting
                delete ptr;
                LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
            }
            return dc.message;
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::pop()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::pop()- exception occured"));
    }

    return LOG4CPLUS_TEXT("");
}


log4cplus::tstring 
NDC::peek()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL && !ptr->empty()) {
            return ptr->top().message;
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::peek()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::peek()- exception occured"));
    }

    return LOG4CPLUS_TEXT("");
}


void 
NDC::push(const log4cplus::tstring& message)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr == NULL) {
            ptr = new DiagnosticContextStack();
            LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, ptr );
        }

        if(ptr->empty()) {
            ptr->push( DiagnosticContext(message, NULL) );
        }
        else {
            DiagnosticContext dc = ptr->top();
            ptr->push( DiagnosticContext(message, &dc) );
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::push()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::push()- exception occured"));
    }
}


void 
NDC::remove()
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            delete ptr;
        }
        LOG4CPLUS_SET_THREAD_LOCAL_VALUE( threadLocal, NULL );
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::remove()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::remove()- exception occured"));
    }
}


void 
NDC::setMaxDepth(unsigned int maxDepth)
{
    try {
        DiagnosticContextStack* ptr = getPtr();
        if(ptr != NULL) {
            while(maxDepth < ptr->size()) {
                ptr->pop();
            }
        }
    }
    catch(std::exception& e) {
        getLogLog().error(  LOG4CPLUS_TEXT("NDC::setMaxDepth()- exception occured: ") 
                          + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
    }
    catch(...) {
        getLogLog().error(LOG4CPLUS_TEXT("NDC::setMaxDepth()- exception occured"));
    }
}


