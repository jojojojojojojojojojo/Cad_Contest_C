#include <cctype> // isspace

#include "util.h"



bool util::read_line_2_tokens(istream &inFile, vector<string> &tokens){
    tokens.clear();

    string line;
    getline(inFile, line);

    while (inFile && tokens.empty()) {

        string token = "";
        for (unsigned i = 0; i < line.size(); ++i) {

            char currChar = line[i];
            if (isspace(currChar) || is_char_special(currChar)) {

                if (!token.empty()) {
                    // Add the current token to the list of tokens
                    tokens.push_back(token);
                    token.clear();
                }
                // else if the token is empty, simply skip the whitespace or special char
            }
            else {
                // Add the char to the current token
                token.push_back(currChar);
            }
        }

        if (!token.empty()){
            tokens.push_back(token);
        }

        if (tokens.empty()){
            // Previous line read was empty. Read the next one.
            getline(inFile, line);
        }
    }

    return !tokens.empty();
}


bool util::is_char_special(char c)
{
  static const char specialChars[] = {'(', ')', ',', ':', ';', '/', '#', '[', ']', '{', '}', '*', '\"', '\\'};

  for( unsigned i = 0; i < sizeof(specialChars); ++i ){
    if (c == specialChars[i]) { return true; }
  }
  return false;
}


void util::GaussianSmooth::smooth(vector<vector<double> > &input)
{
    vector< vector< double > > paddedInput;

    int padSize = (int)m_kernel.size() / 2;
    int height = (int)input.size();
    int width = (int)input[0].size();
    int paddedHeight = height + padSize * 2;
    int paddedWidth  = width + padSize * 2;

    // pre-normalization
    double total = 0;
    for( int i=0; i<height; i++ ){
        for( int j=0; j<width; j++ ){
            total += input[i][j];
        }
    }

    if( total == 0 ) { return; }

    paddedInput.resize( paddedHeight );
    for( int i=0; i<height; i++ )
    {
        paddedInput[i+padSize].resize( paddedWidth );

        for( int j=0; j<padSize; j++ )
            paddedInput[i+padSize][j] = input[i][0];

        for( int j=0; j<width; j++ )
            paddedInput[i+padSize][j+padSize] = input[i][j];

        for( int j=0; j<padSize; j++ )
            paddedInput[i+padSize][j+width+padSize] = input[i][width-1];
    }
    for( int i=0; i<padSize; i++ )
    {
        paddedInput[i] = paddedInput[padSize];
        paddedInput[paddedHeight-i-1] = paddedInput[paddedHeight-padSize-1];
    }


    // 2D convolution
    assert( (int)m_kernel.size() == padSize * 2 + 1 );
    for( int i=0; i<height; i++ )
        for( int j=0; j<width; j++ )
        {
            double sum = 0;
            for( int m=-padSize; m<=padSize; m++ )
                for( int n=-padSize; n<=padSize; n++ )
                    sum += m_kernel[m+padSize][n+padSize] * paddedInput[i+m+padSize][j+n+padSize];
            input[i][j] = sum;
        }

    // normalization
    double new_total = 0;
    for( int i=0; i<height; i++ )
        for( int j=0; j<width; j++ )
            new_total += input[i][j];
    for( int i=0; i<height; i++ )
        for( int j=0; j<width; j++ )
            input[i][j] = input[i][j] * total / new_total;
}

void util::GaussianSmooth::init_gaussian_2D(double theta, int size)
{
    m_kernel.resize( size );
    for( int i=0; i<size; i++ )
    {
        m_kernel[i].resize( size );
        for( int j=0; j<size; j++ )
        {
            m_kernel[i][j] = gaussian_discrete_2D( theta, i-(size/2), j-(size/2) );
        }
    }
}

double util::GaussianSmooth::gaussian_discrete_2D(double theta, int x, int y)
{
    double g = 0;
    for(double ySubPixel = y - 0.5; ySubPixel < y + 0.6; ySubPixel += 0.1)
    {
        for(double xSubPixel = x - 0.5; xSubPixel < x + 0.6; xSubPixel += 0.1)
        {
            g = g + ((1/(2*PI*theta*theta)) *
                     exp( -(xSubPixel*xSubPixel+ySubPixel*ySubPixel)/
                          (2*theta*theta)));
        }
    }
    g = g/121.0;
    return g;
}


void util::smoothing_vec2D(vector2D<double> &vec2D)
{
    GaussianSmooth gs;
    int r = 3;
    gs.init_gaussian_2D( (double)r, 3 );
    vector< vector<double> > tmp( vec2D.xDim(), vector<double>( vec2D.yDim(), 0 ) );
    for( int i=0; i<vec2D.xDim(); ++i ){
        for( int j=0; j<vec2D.yDim(); ++j ){
            tmp[i][j] = vec2D(i,j);
        }
    }
    gs.smooth( tmp );
    for( int i=0; i<vec2D.xDim(); ++i ){
        for( int j=0; j<vec2D.yDim(); ++j ){
            vec2D(i,j) = tmp[i][j];
        }
    }
}
