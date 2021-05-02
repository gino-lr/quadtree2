#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <fstream>
#include <string>

using namespace std;

int profundidad = 10;

class nodo{
public:		
	bool c = false;
	nodo* hojas[4];
	pair<double, double> rect[2];
	nodo(pair<double, double> a[]) {
		rect[0] = a[0];rect[1] = a[1];		
		for (auto& h : hojas) h = NULL;
	}
	nodo(vector<pair<double, double>> a) {
		rect[0] = a[0];rect[1] = a[1];		
		for (auto& h : hojas) h = NULL;
	}
	nodo(nodo& n) {
		rect[0] = n.rect[0];rect[1] = n.rect[1];
		c = n.c;
		for (int i = 0; i < 4; ++i) hojas[i] = n.hojas[i];
	}
	bool H() {
		for (auto& h : hojas) if (h) return false;
		return c;
	}
	vector<pair<double, double>> G(int C) {
		int l1 = 0, l2 = 3;
		if (C < l1 || l2 > 3) return vector<pair<double, double>>();
		vector<pair<double, double>> nrect(2);
		double mid1 = rect[0].first + (rect[0].second - rect[0].first) / 2, mid2 = rect[1].first + (rect[1].second - rect[1].first) / 2; 
		switch (C){
		case 0:
			nrect.assign({ make_pair(rect[0].first, mid1), make_pair(mid2, rect[1].second)});
			break;
		case 1: 
			nrect.assign({ make_pair(mid1, rect[0].second), make_pair(mid2, rect[1].second)});
			break;
		case 2: 
			nrect.assign({ make_pair(rect[0].first, mid1), make_pair(rect[1].first, mid2)});
			break;
		case 3: 
			nrect.assign({ make_pair(mid1, rect[0].second), make_pair(rect[1].first, mid2)});
			break;
		}
		return nrect;
	}
	bool dentro(pair<double, double> P) {
		return (rect[0].first <= P.first && rect[0].second >= P.first) && (rect[1].first <= P.second && rect[1].second >= P.second);
	}	
	int ehoja(pair<double, double> P) {
		if (!dentro(P))
			return -1; 
		double mid1 = rect[0].first + (rect[0].second - rect[0].first) / 2, mid2 = rect[1].first + (rect[1].second - rect[1].first) / 2;
		if ((rect[0].first <= P.first && P.first <= mid1) && (mid2 <= P.second && P.second <= rect[1].second))
			return 0; 		
		if ((mid1 <= P.first && P.first <= rect[0].second) && (mid2 <= P.second && P.second <= rect[1].second))
			return 1; 		
		if ((rect[0].first <= P.first && P.first <= mid1) && (rect[1].first <= P.second && P.second <= mid2))
			return 2; 		
		return 3; 
	}
	~nodo() {
		for (auto& h : hojas) delete h;
	}
};

class quadtree{
public:
	int prof; 
	nodo* r;
	quadtree(pair<double, double> a[],  int b) {
		r = new nodo(a);
		prof = min(20, b);		
	}
	quadtree(vector<pair<double, double>> a,  int b) {
		r = new nodo(a);
		prof = min(20, b);		
	}
	quadtree(nodo R,  int b) {
		r = new nodo(R);
		prof = min(20, b);		
	}
	nodo* getRaiz() {return r;}
	int getProf() {return prof;}
	bool buscar(pair<double, double> P,  int& PP) {
		nodo* pp = r;		
		int C;
		PP = 0;
		if (!pp || !pp->dentro(P))
			return false;				
		for (; !pp->H(); ++PP) {
			C = pp->ehoja(P);
			if (!pp->hojas[C]) return false;
			pp = pp->hojas[C];
		}
		return true;
	}
	bool buscar(pair<double, double> P,  int& PP, stack<nodo**>& pp) {		
		PP = 0;
		int C;
		pp.push(&r);		
		if (!(*pp.top()) || !(*pp.top())->dentro(P)) return false;	
		for (; !(*pp.top())->H(); ++PP) {
			C = (*pp.top())->ehoja(P);
			if (!(*pp.top())->hojas[C]) return false;
			pp.push(&(*pp.top())->hojas[C]);
		}
		return true;
	}
	int insertar(pair<double, double> P) {
		stack<nodo**> pp;
		int PP;
		if (buscar(P, PP, pp)) return -1;
		int C;
		vector<pair<double, double>> nrect;		
		for (; PP < prof - 1; ++PP) {
			C = (*pp.top())->ehoja(P);
			nrect = (*pp.top())->G(C);
			(*pp.top())->hojas[C] = new nodo(nrect);
			pp.push(&(*pp.top())->hojas[C]);
		}
		(*pp.top())->c = true;
		pp.pop();
		while (!pp.empty()) {
			bool lleno = 1; 
			for (auto& h : (*pp.top())->hojas)
				if (!h || !h->H()) lleno = 0; 
			if (lleno) {
				for (auto& h : (*pp.top())->hojas) {
					delete h; 
					h = NULL; 
				}				
				(*pp.top())->c = true;
			}
			pp.pop();
		}
		return PP;
	}
	bool eliminar(pair<double, double> P) {
		stack<nodo**> pp;
		int PP;
		if (!buscar(P, PP, pp)) return false;
		int C;		
		for (; PP < prof - 1; ++PP) {
			C = (*pp.top())->ehoja(P);
			for (int i = 0; i < 4; i++)
				(*pp.top())->hojas[i] = new nodo((*pp.top())->G(i));
			(*pp.top())->c = false;
			pp.push(&(*pp.top())->hojas[C]);
		}
		delete* pp.top();
		*pp.top() = NULL;
		pp.pop();
		while (!pp.empty()) {
			bool vacio = 1;
			for (auto& h : (*pp.top())->hojas)
				if (h) vacio = 0;
			if (vacio && *pp.top() != r) {
				delete* pp.top();
				*pp.top() = NULL;
			}
			pp.pop();
		}
		return true;
	}
	~quadtree() {
		delete r;
	}
};

class area {
public:
	string hex;
	pair<double, double> i1, i2;	
};

vector<area> areaR(quadtree& arbol) {	
	nodo* r = arbol.getRaiz();
	area R;
	vector<area> Q;
	if (r->H()) {
		R.i1 = make_pair(r->rect[1].first, r->rect[0].first);
		R.i2 = make_pair(r->rect[1].second, r->rect[0].second);
		R.hex = "#ffc300";
		Q.push_back(R);
		return Q;
	}else {
		stack<pair<nodo*,  int>> nn;
		nn.push(make_pair(r, 0)); 
		while (!nn.empty()) {
			pair<nodo*,  int> A = nn.top();
			nn.pop();
			for (auto& h : A.first->hojas)			{
				if (h){
					if (h->H()){						
						R.i1 = make_pair(h->rect[1].first, h->rect[0].first);
						R.i2 = make_pair(h->rect[1].second, h->rect[0].second);
						double phoja = static_cast<double>(A.second) + 1.0;
						if (phoja >= 0.9 * static_cast<double>(arbol.getProf()))
							R.hex = "#571845"; 
						else if (phoja >= 0.75 * static_cast<double>(arbol.getProf()))
							R.hex = "#C70039"; 
						/*
						  */
						else
							R.hex = "#ffc300";
						Q.push_back(R);
					}
					else
						nn.push(make_pair(h, A.second + 1)); 
				
	}}}}
	return Q;
}

int main() {
	double XX, YY, RXX[2] = { LDBL_MAX, LDBL_MIN }, RYY[2] = { LDBL_MAX, LDBL_MIN };
	unordered_map<string, string> Rect;
	string cuads = "", X = "html1.txt", l, hh = "";	
	ifstream inicio;	
	inicio.open(X);
	if (inicio.is_open()) {
		while (getline(inicio, l))
			hh += l + "\n";
		inicio.close();
	}	else {
		cout << "ERROR" << X << endl;exit(1);
	}
	ofstream M;
	X = "2021-1 Qtree.html";
	M.open(X);
	M << hh;	
	X = "loc-brightkite_totalCheckins.tsv";
	ifstream TSV;	
	TSV.open(X);
	vector<pair<double, double>> pts;
	if (TSV.is_open()) {		
		while (TSV >> YY >> XX) {
			pts.push_back(make_pair(XX, YY));			
			RXX[0] = min(RXX[0], YY);RXX[1] = max(RXX[1], YY);RYY[0] = min(RYY[0], XX);RYY[1] = max(RYY[1], XX);
		}
		TSV.close();
	}else {
		cout << "ERROR" << X << endl;exit(1);
	}
	pair<double, double> rect[2] = { make_pair(RYY[0], RYY[1]), make_pair(RXX[0], RXX[1]) };
	quadtree arbol(rect, profundidad);
	size_t TSVtam = pts.size();
	for (size_t i = 0; i < TSVtam; ++i) {
		arbol.insertar(pts[i]);
	}
	vector<area> Q = areaR(arbol);		
	Rect["a0"] = "L.rectangle(";Rect["a1"] = ", {color: \"";Rect["a2"] = "\", weight: 1, fillOpacity: 0.2, fill: true}).addTo(mymap);\n";	
	int tam = Q.size();
	for (int i = 0; i < tam; ++i) {
		cuads += Rect["a0"];cuads += "[[" + to_string(Q[i].i1.first) + "," + to_string(Q[i].i1.second) + "],";		
		cuads += "[" + to_string(Q[i].i2.first) + "," + to_string(Q[i].i2.second) + "]]";cuads += Rect["a1"] + Q[i].hex + Rect["a2"];
	}	
	M << cuads; 
	M << "</script></body></html>";
	M.close();
	return 0;
}