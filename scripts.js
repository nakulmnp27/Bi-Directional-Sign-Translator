document.getElementById('input-text').addEventListener('input', convertText);

const supportedExtensions = ['.gif', '.webp', '.jpeg', '.jpg', '.mp4'];
let currentIndex = 0;
let words = [];
let displayInterval;
let errorMessages = {};

function convertText() {
    const inputText = document.getElementById('input-text').value.trim().toLowerCase();
    
    // Convert spaces to underscores for combined file name matching
    let modifiedText = inputText.replace(/\s+/g, '_');

    const displayArea = document.getElementById('display-area');
    displayArea.innerHTML = ''; // Clear existing content
    displayArea.classList.add('fade-in'); // Add fade-in class

    clearInterval(displayInterval);
    errorMessages = {};

    // First, try to display the combined file
    if (!displayMedia(modifiedText)) {
        // If the combined file is not found, split into words and display each separately
        words = inputText.split(' ');
        currentIndex = 0;
        displayNextWord();
    }
}

function displayNextWord() {
    if (words.length === 0) return; // Exit if there are no words

    const word = words[currentIndex];
    const displayArea = document.getElementById('display-area');

    // Clear the display area before displaying the next word (or error)
    displayArea.innerHTML = '';

    if (!displayMedia(word)) {
        // If the word is not found, display an error message
        //displayErrorMessage(word); //Commented to prevent display of error message
    }


    currentIndex = (currentIndex + 1) % words.length; // Move to the next word (circularly)

    // Delay before displaying the next word
    displayInterval = setTimeout(() => {
      //clear previous message before displaying next message
      if (errorMessages[word]) {
            errorMessages[word].remove();
            delete errorMessages[word];
        }
      displayNextWord();
    }, 3000);
}

function displayMedia(fileName) {
    const displayArea = document.getElementById('display-area');
    let foundMedia = false;

    for (let i = 0; i < supportedExtensions.length; i++) {
        const extension = supportedExtensions[i];
        const mediaPath = `signs/${fileName}${extension}`;

        if (extension === '.mp4') {
            const video = document.createElement('video');
            video.src = mediaPath;
            video.className = 'displayed-image';
            video.controls = true;

            video.onloadeddata = function () {
                if (!foundMedia) {
                    displayArea.innerHTML = '';
                    displayArea.appendChild(video);
                    video.play();
                    foundMedia = true;
                }
            };

            video.onerror = () => handleMediaError(fileName);
        } else {
            const img = document.createElement('img');
            img.src = mediaPath;
            img.className = 'displayed-image';

            img.onload = function () {
                if (!foundMedia) {
                    displayArea.innerHTML = '';
                    displayArea.appendChild(img);
                    foundMedia = true;
                }
            };

            img.onerror = () => handleMediaError(fileName);
        }
    }

    if (foundMedia) {
        // If media is found, clear any existing error messages
        if (errorMessages[fileName]) {
            errorMessages[fileName].remove();
            delete errorMessages[fileName];
        }
        return true; // Return true if media was found
    } else {
        return false; // Return false if media was not found
    }
}


function handleMediaError(text) {
   // displayErrorMessage(text); //Commented to prevent display of error message
}

function displayErrorMessage(text) {
    const displayArea = document.getElementById('display-area');

    
}

function removeImage() {
    const displayArea = document.getElementById('display-area');
    displayArea.innerHTML = '';
    clearInterval(displayInterval);

    document.getElementById('input-text').value = '';
    words = [];
    currentIndex = 0;
    errorMessages = {};
}

function startRecognition() {
    if ('webkitSpeechRecognition' in window) {
        const recognition = new webkitSpeechRecognition();
        recognition.lang = 'en-US';
        recognition.interimResults = false;
        recognition.maxAlternatives = 1;

        recognition.onstart = function () {
            console.log('Speech recognition started');
        };

        recognition.onresult = function (event) {
            const transcript = event.results[0][0].transcript.toLowerCase().trim();
            document.getElementById('input-text').value = transcript;
            convertText();
        };

        recognition.onerror = function (event) {
            console.error('Speech recognition error:', event.error);
        };

        recognition.onend = function () {
            console.log('Speech recognition ended');
        };

        recognition.start();
    } else {
        alert('Speech recognition not supported in this browser. Please try using Google Chrome.');
    }
}

// Function to check if a media file exists for the given text
function checkFileExists(fileName) {
    return supportedExtensions.some(extension => {
        let img = new Image();
        img.src = `signs/${fileName}${extension}`;
        return img.complete;
    });
}

// Function to handle voice recognition for the translation input
function startTranslationRecognition() {
    if ('webkitSpeechRecognition' in window) {
        const recognition = new webkitSpeechRecognition();
        recognition.lang = document.getElementById('from-language').value; // Set language from the select element
        recognition.interimResults = false;
        recognition.maxAlternatives = 1;

        recognition.onstart = function () {
            console.log('Translation speech recognition started');
        };

        recognition.onresult = function (event) {
            const transcript = event.results[0][0].transcript.toLowerCase().trim();
            document.getElementById('input-translate').value = transcript;
        };

        recognition.onerror = function (event) {
            console.error('Translation speech recognition error:', event.error);
        };

        recognition.onend = function () {
            console.log('Translation speech recognition ended');
        };

        recognition.start();
    } else {
        alert('Speech recognition not supported in this browser. Please try using Google Chrome.');
    }
}

// New function to clear the translation fields
function clearTranslationFields() {
    document.getElementById('input-translate').value = ''; // Clear the input translation text area
    document.getElementById('output-translate').value = ''; // Clear the output translation text area
}

// Language Translation Using API
async function translateText() {
    const inputText = document.getElementById('input-translate').value;
    const fromLang = document.getElementById('from-language').value;
    const toLang = document.getElementById('to-language').value;

    if (!inputText) {
        alert("Please enter text to translate.");
        return;
    }

    const url = `https://translate.googleapis.com/translate_a/single?client=gtx&sl=${fromLang}&tl=${toLang}&dt=t&q=${encodeURIComponent(inputText)}`;

    try {
        const response = await fetch(url);
        const data = await response.json();

        document.getElementById('output-translate').value = data[0][0][0]; // Extract translated text
    } catch (error) {
        console.error("Error in translation:", error);
        alert("Translation failed");
    }
}
