#include "SDA.h"

SDA::SDA(int numStates, int numChars, int maxRespLen, int outputLen, int initState, bool verbose) {
    initChar = -1;
    this->numStates = numStates;
    this->initState = initState;
    this->numChars = numChars;
    this->maxRespLen = maxRespLen;
    this->outputLen = outputLen;
    this->verbose = verbose;

    transitions.reserve(numStates);
    for (vector<int> v: transitions) {
        v.reserve(numChars);
    }

    responses.reserve(numStates);
    for (vector<vector<int> > vec1: responses) {
        vec1.reserve(numChars);
        for (vector<int> vec2: vec1) {
            vec1.reserve(maxRespLen);
        }
    }
    create();
    if (verbose) cout << "SDA made with " << numStates << " numStates." << endl;
}

SDA::SDA() : SDA(10, 2, 2, 1000) {}

SDA::SDA(SDA &other) {
    copy(other);
}

SDA::~SDA() = default;

int SDA::create() {
    initChar = (int) lrand48() % numChars;

    vector<int> oneState;
    oneState.reserve(numChars);
    for (int state = 0; state < numStates; ++state) {
        oneState.clear();
        for (int val = 0; val < numChars; ++val) {
            oneState.push_back((int) lrand48() % numStates);
        }
        transitions.push_back(oneState);
    }

    vector<int> oneResponse;
    oneResponse.reserve(maxRespLen);
    vector<vector<int>> oneStateResps;
    oneStateResps.reserve(numChars);
    int respSize;
    for (int state = 0; state < numStates; ++state) {
        oneStateResps.clear();
        for (int trans = 0; trans < numChars; ++trans) {
            oneResponse.clear();
            respSize = (int) lrand48() % maxRespLen + 1;
            for (int val = 0; val < respSize; ++val) {
                oneResponse.push_back((int) lrand48() % numChars);
            }
            oneStateResps.push_back(oneResponse);
        }
        responses.push_back(oneStateResps);
    }
    if (verbose) cout << "SDA initialized." << endl;
    return 0;
}

/*
This method sets the length of the bit vector that the SDA will produce
*/

int SDA::setOutputLen(int newLen) {
    this->outputLen = newLen;
    return 0;
}

/*
This method takes an SDA and randomizes the transitions and responses of states in the SDA
*/

int SDA::randomize() {
    if (initChar < 0) {
        cout << "Error in SDA Class: randomize(): this SDA has not been initialized.";
        return -1;
    }

    initChar = (int) lrand48() % numChars;

    vector<int> oneResponse;
    oneResponse.reserve(maxRespLen);
    int respLen;
    for (int state = 0; state < numStates; ++state) {
        for (int trans = 0; trans < numChars; ++trans) {
            transitions[state][trans] = (int) lrand48() % numStates;
            oneResponse.clear();
            respLen = (int) lrand48() % maxRespLen + 1;
            for (int val = 0; val < respLen; ++val) {
                oneResponse.push_back((int) lrand48() % numChars);
            }
            responses[state][trans] = oneResponse;
        }
    }
    if (verbose) cout << "SDA Randomized." << endl;
    return 0;
}

/*
This method makes a copy of an SDA
*/

int SDA::copy(SDA &other) {
    if (initChar < 0) {
        cout << "Error in SDA Class: copy(...): this SDA has not been initialized.";
        return -1;
    }
    if (other.initChar < 0) {
        cout << "Error in SDA Class: copy(...): other SDA has not been initialized.";
        return -1;
    }

    initChar = other.initChar;
    if (numStates != other.numStates) {
        transitions.reserve(other.numStates);
        responses.reserve(other.numStates);
    }
    numStates = other.numStates;
    initState = other.initState;
    if (numChars != other.numChars) {
        for (auto &stateTrans: transitions) {
            stateTrans.reserve(other.numChars);
        }
        for (auto &stateResp: responses) {
            stateResp.reserve(other.numChars);
        }
    }
    numChars = other.numChars;
    if (maxRespLen != other.maxRespLen) {
        for (auto &stateResp: responses) {
            for (auto &resp: stateResp) {
                resp.reserve(other.maxRespLen);
            }
        }
    }
    maxRespLen = other.maxRespLen;
    outputLen = other.outputLen;
    verbose = other.verbose;

    transitions = other.transitions;
    responses = other.responses;
    if (verbose) cout << "SDA Copied." << endl;
    return 0;
}

/*
This method performs a two-point crossover on two SDAs
*/

int SDA::twoPointCrossover(SDA &other) {
    if (initChar < 0) {
        cout << "Error in SDA Class: twoPointCrossover(...): this SDA has not been initialized.";
        return -1;
    }
    if (other.initChar < 0) {
        cout << "Error in SDA Class: twoPointCrossover(...): other SDA has not been initialized.";
        return -1;
    }
    if (numStates != other.numStates) {
        cout << "Error in SDA Class: twoPointCrossover(...): the two SDAs have a different numStates.";
        return 1;
    }
    if (numChars != other.numChars) {
        cout << "Error in SDA Class: twoPointCrossover(...): the two SDAs have a different numChars.";
        return 1;
    }
    if (maxRespLen != other.maxRespLen) {
        cout << "Error in SDA Class: twoPointCrossover(...): the two SDAs have a different maxRespLen.";
        return 1;
    }

    int crossStart, crossEnd, swapInt;
    vector<int> swapVec;
    swapVec.reserve(numChars);

    do {
        crossStart = (int) lrand48() % (numStates + 1);
        crossEnd = (int) lrand48() % (numStates + 1);
        if (crossStart > crossEnd) {
            swapInt = crossStart;
            crossStart = crossEnd;
            crossEnd = swapInt;
        }
    } while (crossStart == crossEnd);

    if (crossStart == 0) {
        swapInt = initChar;
        initChar = other.initChar;
        other.initChar = swapInt;
    }

    for (int state = crossStart; state < crossEnd; state++) {
        swapVec = transitions.at(state);
        transitions.at(state) = other.transitions.at(state);
        other.transitions.at(state) = swapVec;
        for (int trans = 0; trans < numChars; trans++) {
            swapVec = responses.at(state).at(trans);
            responses.at(state).at(trans) = other.responses.at(state).at(trans);
            other.responses.at(state).at(trans) = swapVec;
        }
    }
    return 0;
}

/**
 * This method performs a onestate crossover using two different SADs
 * 
 * @param other is the second SDA the crossover is being performed with
*/


int SDA::oneStateCrossover(SDA &other) {
    if (initChar < 0) {
        cout << "Error in SDA Class: oneStateCrossover(...): this SDA has not been initialized.";
        return -1;
    }
    if (other.initChar < 0) {
        cout << "Error in SDA Class: oneStateCrossover(...): other SDA has not been initialized.";
        return -1;
    }
    if (numStates != other.numStates) {
        cout << "Error in SDA Class: oneStateCrossover(...): the two SDAs have a different numStates.";
        return 1;
    }
    if (numChars != other.numChars) {
        cout << "Error in SDA Class: oneStateCrossover(...): the two SDAs have a different numChars.";
        return 1;
    }
    if (maxRespLen != other.maxRespLen) {
        cout << "Error in SDA Class: oneStateCrossover(...): the two SDAs have a different maxRespLen.";
        return 1;
    }

    int crossState, swapInt;
    vector<int> swapVec;

    crossState = (int) lrand48() % numStates;
    if (crossState == 0) {
        swapInt = initChar;
        initChar = other.initChar;
        other.initChar = swapInt;
    }

    swapVec = transitions.at(crossState);
    transitions.at(crossState) = other.transitions.at(crossState);
    other.transitions.at(crossState) = swapVec;
    for (int trans = 0; trans < numChars; trans++) {
        swapVec = responses.at(crossState).at(trans);
        responses.at(crossState).at(trans) = other.responses.at(crossState).at(trans);
        other.responses.at(crossState).at(trans) = swapVec;
    }
    return 0;
}

/**
 * This method performs mutations on the SDA, which involves swapping either the transitions or responses, this has a
 * 50% chance of occuring if one does not happen than the other does.
 * 
 * @param numMuts is the number of mutations that will be performed on the SDA
*/

int SDA::mutate(int numMuts) {
    if (initChar < 0) {
        cout << "Error in SDA Class: mutate(...): this SDA has not been initialized.";
        return -1;
    }

    int mutPt, respSize;
    vector<int> oneResponse;

    for (int mut = 0; mut < numMuts; ++mut) {
        if (drand48() < 0.04) { // 4% chance of mutating initial character
            initChar = (int) lrand48() % numChars;
            return 0;
        } else {
            mutPt = (int) lrand48() % numStates;
            int transNum = (int) lrand48() % numChars;

            if ((int) lrand48() % 2 == 0) { // Mutate transition (50%)
                transitions.at(mutPt).at(transNum) = (int) lrand48() % numStates;
            } else { // Mutate response (50%)
                oneResponse.clear();
                respSize = (int) lrand48() % maxRespLen + 1;
                for (int i = 0; i < respSize; ++i) {
                    oneResponse.push_back((int) lrand48() % numChars);
                }
                responses.at(mutPt).at(transNum) = oneResponse;
            }
        }
    }
    return 0;
}

/**
 * This method initalizes the SDA to its initial state, adds the initial character
 * to a vector and uses the added elements to transition from state to state,
 * appending the responses to the vector until the vector contains "outputlen"
 * number of elements.
 * 
 * @param output is the vector containg the transitions from one state in the SDA to another
 * @param printToo Is a boolean value detemring if the SDA is printed to the designated output stream
 * @param outstream The designated output stream the generated SDA values are printed
*/

int SDA::fillOutput(vector<int> &output, bool printToo, ostream &outStream) {
    if (initChar < 0) {
        cout << "Error in SDA Class: fillOutput(...): this SDA has not been initialized.";
        return -1;
    }
    if (output.capacity() < outputLen) {
        cout << "Error in SDA Class: fillOutput(...): output vector capacity is " << output.capacity();
        cout << " but the outputLen is " << outputLen << "." << endl;
        return -1;
    }

    int headIdx = 0;
    int tailIdx = 0;
    curState = initState;
    output[headIdx++] = initChar;
    if (printToo) outStream << initChar;// print the inital character entering the SDA

    while (headIdx < outputLen) {
        for (int val: responses[curState][output[tailIdx]]) {
            if (headIdx < outputLen) {
                output[headIdx++] = val;
                if (printToo) outStream << " " << val;// print the response associated with a state
            }
        }
        curState = transitions[curState][output[tailIdx++]];
    }
    if (printToo) outStream << endl;// create new line
    return 0;
}

/**
 * This method returns the ouput associated with this SDA in a vector, and prints it to a designated output if
 * designated so by the user
 * 
 * @param prinToo the boolean value determing if the resulting output is printed to the designated output stream
 * @param ostream designates where the ouput from the SDA will be printed
 * 
*/

vector<int> SDA::rtnOutput(bool printToo, ostream &outStream) {
    if (initChar < 0) {
        cout << "Error in SDA Class: rtnOutput(...): this SDA has not been initialized.";
        return {-1};
    }

    vector<int> output(outputLen);
    fillOutput(output, printToo, outStream);
    return output;
}

/**
 * This method prints the SDA currently stored in the class
 * 
 * @param ostream is the destination the informaiton from the method is being printed
*/

int SDA::printSDA(ostream &outStream = cout) {
    if (initChar < 0) {
        cout << "Error in SDA Class: printSDA(...): this SDA has not been initialized.";
        return -1;
    }

    outStream << initState << " <- " << initChar << endl;
    for (int state = 0; state < numStates; ++state) {
        for (int trans = 0; trans < numChars; ++trans) {
            outStream << state << " + " << trans << " -> " << transitions[state][trans] << " [";
            for (int vec: responses[state][trans]) {
                outStream << " " << vec;
            }
            outStream << " ]" << endl;
        }
    }
    if (verbose) cout << "SDA Printed." << endl;
    return 0;
}