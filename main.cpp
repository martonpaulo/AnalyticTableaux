#include "AnalyticTableaux.h"
#include "AnalyticTableaux.cpp"




bool solve(string expression, int checkType);
void makeTree(Node &node);
void makeTreeVertical(Node &node);
int analyseExpression(Node node, string &expression1, bool &truthValue1, string &expression2, bool &truthValue2);
char divideExpression(string expression, string &expression1, string &expression2);
void replaceString(string& str, const string& from, const string& to);





int main(){
	
	int N, count, checkType, delimiter, expressionQty;
	size_t pos, posComma, posConsequenciaLogica;
	bool found;
	string checkName, expression, question, str;


	//le a quantidade de expressoes
	scanf("%d ", &N);
	count = 0;


	//faz a leitura das expressoes N vezes ou ate quando tiver linha para ler
	while(getline(cin, question) && count<N){
		
		//separa a expressao
		str = " e ";
		pos = question.find(str);
		delimiter = static_cast<int>(pos);
		expression = question.substr(0, delimiter);
		checkName = question.substr(delimiter, question.size() - delimiter);
		//cout << "expression = " << expression << endl;

		//separa o tipo de checagem
		str = "?";
		pos = checkName.find(str);
		delimiter = static_cast<int>(pos);
		checkName = checkName.substr(0, delimiter);
		replaceString(checkName, " e ", "");
		//cout << "checkName = " << checkName << endl;

		//para evitar casos em que nao eh nenhum tipo de checagem
		posConsequenciaLogica = checkName.find("consequencia logica");


		//para cada tipo de checagem, atribui um numero
		if(!checkName.compare("satisfativel")){
			checkType = 1;

		}else if(!checkName.compare("refutavel")){
			checkType = 2;

		}else if(!checkName.compare("tautologia")){
			checkType = 3;

		}else if(!checkName.compare("insatisfativel")){
			checkType = 4;

		//para consequencia logica, reune todas as expressoes numa expressao soh e faz a seguinte equivalencia:
		//A e consequencia logica de {B, C}?
		//((B & C) > A) e tautologia?
		}else if(posConsequenciaLogica!=string::npos){
			
			checkType = 5;

			str = checkName;
			checkName = "consequencia logica";

			replaceString(str, "consequencia logica de {", "");
			replaceString(str, "}", "");

			found = false;

			while((posComma = str.find(", ")) != string::npos){
				
				if(found){
					str.replace(posComma, 2, ") & ");
					str = "(" + str;
				}else{
					str.replace(posComma, 2, " & ");
				}
				found = true;
			}

			if(found) str = "(" + str + ")";
			
			str = "(" + str + " > " + expression + ")";
			expression = str;
		}
	
		//cout << "expression = " << expression << endl;
	
		//imprime a resposta, enquanto chama a funcao principal
		cout << "Problema #" << ++count << endl << (solve(expression, checkType) ? "Sim," : "Nao, nao");
		cout << " e " << checkName << "." << endl << endl;	
	}

	return 0;
}






bool solve(string expression, int checkType){

	/*
	No 	TIPO DE CHECAGEM 	IGUALAR A 	ARVORE
	1 	satisfativel 		1 			aberta
	2	refutavel 			0 			aberta
	3	tautologia 			0 			fechada
	4 	insatisfativel 		1 			fechada

	OBS.: consequencia logica eh convertida para tautologia
	*/


	Node tableau = (checkType == 1 || checkType == 4) ? Node(expression, true) : Node(expression, false);

	makeTree(tableau);

	//tableau.printTree();
	
	if(checkType == 1 || checkType == 2) 
		return (tableau.isClosed() ? false : true);
	else
		return (tableau.isClosed() ? true : false);
}






void makeTree(Node &node){

	int i, j, expressionType;
	bool truthValue, truthValue1, truthValue2;
	string expression, expression1, expression2;
	vector<Node*> appliableNodes, insertedNodes;
	
	expression = node.getExpression();
	truthValue = node.getTruthValue();

	//faz a arvore vertical
	makeTreeVertical(node);

	appliableNodes = node.getAppliableNodes();
	
	
	for(i=0; i<appliableNodes.size(); i++){
		
		//acrescenta as bifurcacoes
		analyseExpression((*appliableNodes[i]), expression1, truthValue1, expression2, truthValue2);
		insertedNodes = node.insertSides(expression1, truthValue1, expression2, truthValue2);
		(*appliableNodes[i]).markApplied();

		//para cada bifurcacao, retorna e faz a arvore vertical
		for(j=0; j<insertedNodes.size(); j++){
			makeTree(*insertedNodes[j]);
		}
	}
	
}








void makeTreeVertical(Node &node){

	int i, expressionType;
	bool truthValue, truthValue1, truthValue2, swapped;
	string expression, expression1, expression2;
	vector<Node*> insertedNodes;
	
	expression = node.getExpression();
	truthValue = node.getTruthValue();


	expressionType = analyseExpression(node, expression1, truthValue1, expression2, truthValue2);
	
	if(expressionType==1){
		if(node.checkContradiction()){
			node.markContradiction();
			return;
		}

	}else if(expressionType==2){
		insertedNodes = node.insertFront(expression1, !truthValue);
		node.markApplied();

	}else{
		if(expression2.size()==1){
			swap(expression1, expression2);
			swap(truthValue1, truthValue2);
		}
		if(expressionType==3){
			insertedNodes = node.insertFront(expression1, truthValue1, expression2, truthValue2);
			node.markApplied();
		}
	}

	
	for(i=0; i<insertedNodes.size(); i++){
		makeTreeVertical(*insertedNodes[i]);
	}
}





int analyseExpression(Node node, string &expression1, bool &truthValue1, string &expression2, bool &truthValue2){

	char operation;
	bool truthValue = node.getTruthValue();
	string expression = node.getExpression();
	
	
	//se a expressao for atomica
	if(expression.size()==1){
		return 1;
	
	}else{

		//se a regra for NAO
		if(!(expression.substr(0, 2)).compare("(~")){
			expression1 = expression.substr(2, expression.size()-3);
			return 2;

		}else{

			operation = divideExpression(expression, expression1, expression2);


			//REGRAS TIPO A

			if(operation == '&' && truthValue){
				truthValue1 = true;
				truthValue2 = true;
				return 3;

			}else if(operation == 'v' && !truthValue){
				truthValue1 = false;
				truthValue2 = false;
				return 3;

			}else if(operation == '>' && !truthValue){
				truthValue1 = true;
				truthValue2 = false;
				return 3;
		

			//REGRAS TIPO B

			}else if(operation == '&' && !truthValue){
				truthValue1 = false;
				truthValue2 = false;
				return 4;

			}else if(operation == 'v' && truthValue){
				truthValue1 = true;
				truthValue2 = true;
				return 4;
			
			}else if(operation == '>' && truthValue){
				truthValue1 = false;
				truthValue2 = true;
				return 4;
			}
		}
	}
}






char divideExpression(string expression, string &expression1, string &expression2){

	if(expression.empty()) return ' ';

	int insideParentesis = 0, i=0;
	bool found = false;
	char operation;

	for(string::iterator it = expression.begin(); it != expression.end() && !found; ++it, i++){
	    
	    if((*it) == '('){
	    	insideParentesis++;

	    }else if((*it) == ')'){
	    	insideParentesis--;
	    
	    }else if(insideParentesis==1 && ( ((*it) == '&') || ((*it) == 'v') || ((*it) == '>')) ){
    		found = true;
    		operation = *it;
	    }

	}
	
	expression1 = expression.substr(1, i-3);
	expression2 = expression.substr(i+1, expression.size()-i-2);
		
	return operation;
}




void replaceString(string &str, const string &from, const string &to){
	
	if(from.empty()) return;

	size_t pos = 0;
	if((pos = str.find(from, pos)) != string::npos)
		str.replace(pos, from.length(), to);
}