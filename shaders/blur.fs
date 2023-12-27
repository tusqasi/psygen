#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// NOTE: Render size values must be passed from code
const float renderWidth = 1000;
const float renderHeight = 1000;

const vec2 resolution = vec2(1000,1000);

const float blurRadius = 3; // radius of the blur

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
    // Calculate the Gaussian kernel
    const int kernelSize = 15;
    float kernel[kernelSize];
    float sigma = blurRadius;
    float twoSigmaSquared = 2.0 * sigma * sigma;
    float rootTwoPiSigma = sqrt(2.0 * 3.14159265358979323846) * sigma;
    
    float total = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        float x = float(i - (kernelSize - 1) / 2);
        kernel[i] = exp(-x * x / twoSigmaSquared) / rootTwoPiSigma;
        total += kernel[i];
    }
    
    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i)
    {
        kernel[i] /= total;
    }

    // Horizontal blur
    vec3 blurColorH = vec3(0.0);
    for (int i = 0; i < kernelSize; ++i)
    {
        float offset = float(i - (kernelSize - 1) / 2);
        vec2 offsetCoordsH = fragTexCoord + vec2(offset / resolution.x, 0.0);
        blurColorH += texture(texture0, offsetCoordsH).rgb * kernel[i];
    }

    // Vertical blur
    vec3 blurColorV = vec3(0.0);
    for (int i = 0; i < kernelSize; ++i)
    {
        float offset = float(i - (kernelSize - 1) / 2);
        vec2 offsetCoordsV = fragTexCoord + vec2(0.0, offset / resolution.y);
        blurColorV += texture(texture0, offsetCoordsV).rgb * kernel[i];
    }

    // Combine the results
    finalColor = vec4(blurColorH + blurColorV, 1.0);
}

