/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
The MIT License (MIT)

Copyright (c) <year> 2007-23 Mike Karlesky, Mark VanderVoord, Greg Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 * ========================================== */

#ifndef UNITY_FIXTURE_INTERNALS_H_
#define UNITY_FIXTURE_INTERNALS_H_

#ifdef __cplusplus
extern "C"
{
#endif

struct UNITY_FIXTURE_T
{
    int Verbose;
    int Silent;
    unsigned int RepeatCount;
    const char* NameFilter;
    const char* GroupFilter;
};
extern struct UNITY_FIXTURE_T UnityFixture;

typedef void unityfunction(void);
void UnityTestRunner(unityfunction* setup,
                     unityfunction* testBody,
                     unityfunction* teardown,
                     const char* printableName,
                     const char* group,
                     const char* name,
                     const char* file, unsigned int line);

void UnityIgnoreTest(const char* printableName, const char* group, const char* name);
int UnityGetCommandLineOptions(int argc, const char* argv[]);
void UnityConcludeFixtureTest(void);

void UnityPointer_Set(void** pointer, void* newValue, UNITY_LINE_TYPE line);
void UnityPointer_UndoAllSets(void);
void UnityPointer_Init(void);
#ifndef UNITY_MAX_POINTERS
#define UNITY_MAX_POINTERS 5
#endif

#ifdef __cplusplus
}
#endif

#endif /* UNITY_FIXTURE_INTERNALS_H_ */
