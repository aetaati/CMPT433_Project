const fs = require('fs');
const { join } = require('path');

console.log('Copying the rhino models...');

const outputDirectory = join(__dirname, '..', 'test');

const paramsSourceDirectory = join(
  __dirname,
  '..',
  '..',
  '..',
  'lib',
  'common',
);

const testDataSource = join(
  __dirname,
  '..',
  '..',
  '..',
  'resources',
  'test',
  'test_data.json'
);


console.log("Copying the RHN model...");

const sourceDirectory = join(
  __dirname,
  "..",
  "..",
  "..",
  "resources",
);

try {
  fs.mkdirSync(outputDirectory, { recursive: true });

  fs.readdirSync(paramsSourceDirectory).forEach(file => {
    fs.copyFileSync(join(paramsSourceDirectory, file), join(outputDirectory, file));
  });

  fs.copyFileSync(testDataSource, join(outputDirectory, 'test_data.json'));

  fs.mkdirSync(join(outputDirectory, 'contexts'), { recursive: true });
  fs.readdirSync(sourceDirectory).forEach(folder => {
    if (folder.includes("contexts")) {
      fs.readdirSync(join(sourceDirectory, folder, 'wasm')).forEach(file => {
        fs.copyFileSync(join(sourceDirectory, folder, 'wasm', file), join(outputDirectory, 'contexts', file));
      })
    }
  });

  fs.mkdirSync(join(outputDirectory, 'audio_samples'), { recursive: true });
  fs.readdirSync(join(sourceDirectory, 'audio_samples')).forEach(file => {
    fs.copyFileSync(join(sourceDirectory, 'audio_samples', file), join(outputDirectory, 'audio_samples', file));
  });
} catch (error) {
  console.error(error);
}

console.log('... Done!');