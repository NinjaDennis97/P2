package server;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;


public class ServerController {
	private int port = 3500;
	public UI ui;
	private Random rand = new Random();
	private List<String> list16 = new ArrayList<>();
	private List<String> list15 = new ArrayList<>();
	private List<String> list14 = new ArrayList<>();
	private List<String> list13 = new ArrayList<>();
	private List<String> list12 = new ArrayList<>();
	private List<String> list11 = new ArrayList<>();
	private List<String> list10 = new ArrayList<>();
	private List<String> list9 = new ArrayList<>();
	private List<String> list8 = new ArrayList<>();
	private List<String> list7 = new ArrayList<>();
	private List<String> list6 = new ArrayList<>();
	private List<String> list5 = new ArrayList<>();
	private List<String> list4 = new ArrayList<>();
	private List<String> list3 = new ArrayList<>();
	private List<String> list2 = new ArrayList<>();
	private String word1, word2;
	private int connectedNodesInt;
	ServerConnectionV2 sv;

	public ServerController () {
		ui = new UI(this);
		wordReader();
		sv = new ServerConnectionV2(port, this);
	}
	
	

	public void wordReader() {

		try {
			File file = new File("Ordlista.txt");
			FileReader fr = new FileReader(file);
			BufferedReader br = new BufferedReader(fr);
			StringBuffer sb = new StringBuffer();
			String line;
			while((line=br.readLine())!= null) {
				if(line.length() == 16) {
					list16.add(line);
				}else if(line.length() == 15) {
					list15.add(line);
				}else if(line.length() == 14) {
					list14.add(line);
				}else if(line.length() == 13) {
					list13.add(line);
				}else if(line.length() == 12) {
					list12.add(line);
				}else if(line.length() == 11) {
					list11.add(line);
				}else if(line.length() == 10) {
					list10.add(line);
				}else if(line.length() == 9) {
					list9.add(line);
				}else if(line.length() == 8) {
					list8.add(line);
				}else if(line.length() == 7) {
					list7.add(line);
				}else if(line.length() == 6) {
					list6.add(line);
				}else if(line.length() == 5) {
					list5.add(line);
				}else if(line.length() == 4) {
					list4.add(line);
				}else if(line.length() == 3) {
					list3.add(line);
				}else if(line.length() == 2) {
					list2.add(line);
				}
			//	ui.writeLog(line);
			}
//			test();
//			ui.writeLog(Integer.toString(list16.size()));
//			ui.writeLog(Integer.toString(list11.size()));
			connectedNodesInt = 10;
//			selectWords();
//			ui.writeLog(word1 + " och " + word2 + " Det blir " + (word1.length()+ word2.length()-1));
//			selectWords();
//			ui.writeLog(word1 + " och " + word2 + " Det blir " + (word1.length()+ word2.length()-1));
//			selectWords();
//			ui.writeLog(word1 + " och " + word2 + " Det blir " + (word1.length()+ word2.length()-1));
//			selectWords();
//			ui.writeLog(word1 + " och " + word2 + " Det blir " + (word1.length()+ word2.length()-1));
//			selectWords();
//			ui.writeLog(word1 + " och " + word2 + " Det blir " + (word1.length()+ word2.length()-1));
//			selectWords();
			
//			if(word2.length() > 0)
//				ui.setCorrectWord(word1 + ", " + word2);
//			else
//				ui.setCorrectWord(word1);
	}

		catch(IOException e) {
			
		}
	}
	
	public void selectWords() {
		word2 = null;
		char c = '!';
		do {
			word1 = getOnedWord(connectedNodesInt);
			String word1PossibleChars = word1;
			if(connectedNodesInt - word1.length() > 0) {
				while(word2 == null && word1PossibleChars.length() > 0) {
					int rndIndex = rand.nextInt(word1PossibleChars.length());
					c = word1PossibleChars.charAt(rndIndex);
					word1PossibleChars = word1PossibleChars.replaceAll(Character.toString(c), "");
					word2 = getWordWithChar(c, connectedNodesInt - word1.length()+1);
//					System.out.println(word1PossibleChars);
				}
			}else word2 = "";
		}while(word2 == null);
		ui.writeLog("gemensambokstav är: " + Character.toString(c));
		System.out.println(word1 + ", " + word2);
		prepareWords(word1,word2,c);
	}
	
	private void prepareWords(String word1, String word2, char commonLetter) {
		String fullWord = "";
		int totalChars = 0; //Ifall word2 = null blir det fel på längden då?
		int i = 0;
		int counter = 0;
		int letterCounter = 0;
		char c = '!';
		String scrambledWord = "";
		if (word2 == "") {
			fullWord = word1;
			totalChars = word1.length();
			counter = totalChars;
			
		} else {
			fullWord = word1+word2;
			totalChars = word1.length() + word2.length();
			counter = totalChars;
		}
		do {
			
			int randomIndex = rand.nextInt(counter);
			c = fullWord.charAt(randomIndex);
			if (c == commonLetter) {
				for(int j = 0; j < fullWord.length(); j++) {
					if (fullWord.charAt(j) == c) {
						letterCounter++;
						scrambledWord = scrambledWord + c;
					}
				}
				fullWord = fullWord.replaceAll(Character.toString(c), ""); //Hur löser mn problemrt om orden är typ kontroll och tull?
				i = i+letterCounter;
				counter = counter - letterCounter;
			}else {
				fullWord = fullWord.replaceFirst(Character.toString(c), "");
				counter--;
				i++;
				scrambledWord = scrambledWord + c;
			}
			
			//måste kolla ifall bokstaven finns i båda orden eller ej.
			
		}while(i < totalChars);
		sendCharToNode(scrambledWord);
		}


	private void sendCharToNode(String scrambledWord) {
		System.out.println(scrambledWord);
		sv.sendCharToEveryNode(scrambledWord);
	}


	public String getWordWithChar (char c, int lenOfWord) {
		int rndIndex;
		List<String>wordsWithChar = new ArrayList<String>();
		switch (lenOfWord) {

		case 16: 
			for(int i = 0; i < list16.size(); i++) {
				if(list16.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list16.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;

		case 15: 
			for(int i = 0; i < list15.size(); i++) {
				if(list15.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list15.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 14: 
			for(int i = 0; i < list14.size(); i++) {
				if(list14.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list14.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;


		case 13: 
			for(int i = 0; i < list13.size(); i++) {
				if(list13.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list13.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 12: 
			for(int i = 0; i < list12.size(); i++) {
				if(list12.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list12.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;


		case 11: 
			for(int i = 0; i < list11.size(); i++) {
				if(list11.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list11.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 10: 
			for(int i = 0; i < list10.size(); i++) {
				if(list10.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list10.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 9: 
			for(int i = 0; i < list9.size(); i++) {
				if(list9.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list9.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 8: 
			for(int i = 0; i < list8.size(); i++) {
				if(list8.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list8.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		case 7: 
			for(int i = 0; i < list7.size(); i++) {
				if(list7.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list7.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
			
		case 6: 
			for(int i = 0; i < list6.size(); i++) {
				if(list6.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list6.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
			
		case 5: 
			for(int i = 0; i < list5.size(); i++) {
				if(list5.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list5.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
			
		case 4: 
			for(int i = 0; i < list4.size(); i++) {
				if(list4.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list4.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
			
		case 3: 
			for(int i = 0; i < list3.size(); i++) {
				if(list3.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list3.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
			
		case 2: 
			for(int i = 0; i < list2.size(); i++) {
				if(list2.get(i).indexOf(c) != -1) {
					wordsWithChar.add(list2.get(i));
				}
			}
			if(!wordsWithChar.isEmpty()) {
				rndIndex = rand.nextInt(wordsWithChar.size());
				return wordsWithChar.get(rndIndex);
			}
			break;
		}


		return null;
	}

	public String getOnedWord(int node) {
		int rndList;
		int wordLen = rand.nextInt(node - 1) + 2;
		if(wordLen == 16) {
			rndList = rand.nextInt(list16.size());
			return list16.get(rndList);
		}else if(wordLen == 15) {
			rndList = rand.nextInt(list15.size());
			return list15.get(rndList);
		}else if(wordLen == 14) {
			rndList = rand.nextInt(list14.size());
			return list14.get(rndList);
		}else if(wordLen == 13) {
			rndList = rand.nextInt(list13.size());
			return list13.get(rndList);
		}else if(wordLen == 12) {
			rndList = rand.nextInt(list12.size());
			return list12.get(rndList);
		}else if(wordLen == 11) {
			rndList = rand.nextInt(list11.size());
			return list11.get(rndList);
		}else if(wordLen == 10) {
			rndList = rand.nextInt(list10.size());
			return list10.get(rndList);
		}else if(wordLen == 9) {
			rndList = rand.nextInt(list9.size());
			return list9.get(rndList);
		}else if(wordLen == 8) {
			rndList = rand.nextInt(list8.size());
			return list8.get(rndList);
		}else if(wordLen == 7) {
			rndList = rand.nextInt(list7.size());
			return list7.get(rndList);
		}else if(wordLen == 6) {
			rndList = rand.nextInt(list6.size());
			return list6.get(rndList);
		}else if(wordLen == 5) {
			rndList = rand.nextInt(list5.size());
			return list5.get(rndList);
		}else if(wordLen == 4) {
			rndList = rand.nextInt(list4.size());
			return list4.get(rndList);
		}else if(wordLen == 3) {
			rndList = rand.nextInt(list3.size());
			return list3.get(rndList);
		}else if(wordLen == 2) {
			rndList = rand.nextInt(list2.size());
			return list2.get(rndList);
		}
		
		return Integer.toString(wordLen);
	}
	
	public void test() {
//		ui.writeLog(getWordWithChar('l', 8));
//		ui.writeLog(getWordWithChar('l', 10));
//		ui.writeLog(getWordWithChar('!', 5));
//		ui.writeLog(getWordWithChar('t', 4));
//		ui.writeLog(getWordWithChar('t', 13));
//		ui.writeLog(getOnedWord(16));
//		ui.writeLog(getOnedWord(10));
//		ui.writeLog(getOnedWord(5));
//		ui.writeLog(getOnedWord(3));
//		ui.writeLog(getOnedWord(9));
		connectedNodesInt = 6;
		selectWords();
		ui.writeLog(word1 + " och " + word2 );
		connectedNodesInt = 8;
		selectWords();
		ui.writeLog(word1 + " och " + word2 );
		connectedNodesInt = 15;
		selectWords();
		ui.writeLog(word1 + " och " + word2 );
		connectedNodesInt = 4;
		selectWords();
		ui.writeLog(word1 + " och " + word2 );
		connectedNodesInt = 10;
		selectWords();
		ui.writeLog(word1 + " och " + word2 );
		
	}
}
