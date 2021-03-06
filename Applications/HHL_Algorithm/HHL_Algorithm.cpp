/*
Copyright (c) 2017-2018 Origin Quantum Computing. All Right Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "Core/Core.h"

using namespace std;
using namespace QPanda;

QCircuit CRotate(vector<Qubit*> qVec)
{
    QCircuit CRot = CreateEmptyCircuit();
    vector<Qubit*> controlVector;
    controlVector.push_back(qVec[1]);
    controlVector.push_back(qVec[2]);
    QGate gat4 = RY(qVec[0], PI);
    gat4.setControl(controlVector);
    QGate gat5 = RY(qVec[0], PI / 3);
    gat5.setControl(controlVector);
    QGate gat6 = RY(qVec[0], 0.679673818908); // arcsin(1/3)
    gat6.setControl(controlVector);
    CRot << X(qVec[1]) << gat4 << X(qVec[1]) << X(qVec[2]) << gat5 << X(qVec[2]) << gat6;
    return CRot;
}

QCircuit hhlPse(vector<Qubit*> qVec)
{
    QCircuit PSEcircuit = CreateEmptyCircuit();
    PSEcircuit << H(qVec[1]) << H(qVec[2]) << RZ(qVec[2], 0.75 * PI);
    QGate gat1 = CU(PI, 1.5 * PI, -0.5 * PI, PI / 2, qVec[2], qVec[3]);
    QGate gat2 = CU(PI, 1.5 * PI, -PI, PI / 2, qVec[1], qVec[3]);
    PSEcircuit << gat1 << RZ(qVec[1], 1.5 * PI) << gat2;
    PSEcircuit << CNOT(qVec[1], qVec[2]) << CNOT(qVec[2], qVec[1]) << CNOT(qVec[1], qVec[2]);
    QGate gat3 = CU(-0.25 * PI, -0.5 * PI, 0, 0, qVec[2], qVec[1]);
    PSEcircuit << H(qVec[2]) << gat3 << H(qVec[1]); // PSE over
    return PSEcircuit;
}

QProg hhl_no_measure(vector<Qubit*> qVec, vector<ClassicalCondition> cVec)
{
    QCircuit PSEcircuit = hhlPse(qVec); // PSE
    QCircuit CRot = CRotate(qVec); // control-lambda

    QCircuit PSEcircuitdag = hhlPse(qVec);
    QProg PSEdagger = CreateEmptyQProg();
    PSEdagger << PSEcircuitdag.dagger();
    QIfProg ifnode = CreateIfProg(cVec[0], PSEdagger);
    QProg hhlProg = CreateEmptyQProg();

    hhlProg << PSEcircuit << CRot << Measure(qVec[0], cVec[0]) << ifnode;
    return hhlProg;
}

int main()
{
    map<string, bool> temp;
    int x0 = 0;
    int x1 = 0;

    init(QMachineType::CPU);
    int qubit_number = 4;
    vector<Qubit*> qv = qAllocMany(qubit_number);
    int cbitnum = 2;
    vector<ClassicalCondition> cv = cAllocMany(2);

    auto hhlprog = CreateEmptyQProg();
    hhlprog << RY(qv[3], PI / 2); //  change vecotr b in equation Ax=b
    hhlprog << hhl_no_measure(qv, cv);
    directlyRun(hhlprog);
    QVec pmeas_q;
    pmeas_q.push_back(qv[3]);
    vector<double> s = PMeasure_no_index(pmeas_q);

    cout << "prob0:" << s[0] << endl;
    cout << "prob1:" << s[1] << endl;
    finalize();
}
