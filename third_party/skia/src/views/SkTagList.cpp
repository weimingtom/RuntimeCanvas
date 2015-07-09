
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkTagList.h"

SkTagList::~SkTagList()
{
}

SkTagList* SkTagList::Find(SkTagList* rec, U8CPU tag)
{
    while (rec != NULL)
    {
        if (rec->fTag == tag)
            break;
        rec = rec->fNext;
    }
    return rec;
}

void SkTagList::DeleteTag(SkTagList** head, U8CPU tag)
{
    SkTagList* rec = *head;
    SkTagList* prev = NULL;

    while (rec != NULL)
    {
        SkTagList* next = rec->fNext;

        if (rec->fTag == tag)
        {
            if (prev)
                prev->fNext = next;
            else
                *head = next;
            delete rec;
            break;
        }
        prev = rec;
        rec = next;
    }
}

void SkTagList::DeleteAll(SkTagList* rec)
{
    while (rec)
    {
        SkTagList* next = rec->fNext;
        delete rec;
        rec = next;
    }
}
