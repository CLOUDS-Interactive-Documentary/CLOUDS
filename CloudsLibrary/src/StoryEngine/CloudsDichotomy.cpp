//
//  keywordDichotomy.cpp
//  CloudsStoryEngine
//
//  Created by Surya Mattu on 22/07/13.
//
//

#include "CloudsDichotomy.h"

CloudsDichotomy::CloudsDichotomy(){
    left = "";
    right = "";
    balance = 0; //pos/neg
}


vector<CloudsDichotomy> CloudsDichotomy::getDichotomies(){

	vector<CloudsDichotomy> dichotomies;

	CloudsDichotomy d;
	d.left = "#art";
	d.right = "#tech";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#emotional";
	d.right = "#logical";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#breakthrough";
	d.right = "#obstacle";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#inspiring";
	d.right = "#discouraging";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#fun";
	d.right = "#serious";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#sincere";
	d.right = "#ironic";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#mindblowing";
	d.right = "#mundane";
	d.balance =0;
	dichotomies.push_back(d);
	
	d.left = "#rational";
	d.right = "#surreal";
	d.balance =0;
	dichotomies.push_back(d);

	return dichotomies;
}