Translations
============

Donu has been designed to support multiple localisations. This makes adding new phrases, and completely new languages easily achievable. For managing all application translations, Donu makes use of the Transifex online translation management tool.

### Writing code with translations
We use automated scripts to help extract translations in both Qt, and non-Qt source files. It is rarely necessary to manually edit the files in `src/qt/locale/`. The translation source files must adhere to the following format:
`bitcoin_xx_YY.ts or bitcoin_xx.ts`

`src/qt/locale/bitcoin_en.ts` is treated in a special way. It is used as the source for all other translations. Whenever a string in the source code is changed, this file must be updated to reflect those changes. A custom script is used to extract strings from the non-Qt parts. This script makes use of `gettext`, so make sure that utility is installed (ie, `apt-get install gettext` on Ubuntu/Debian). Once this has been updated, `lupdate` (included in the Qt SDK) is used to update `bitcoin_en.ts`.

To automatically regenerate the `bitcoin_en.ts` file, run the following commands:
```sh
cd src/
make translate
```

`contrib/bitcoin-qt.pro` takes care of generating `.qm` (binary compiled) files from `.ts` (source files) files. It’s mostly automated, and you shouldn’t need to worry about it.

**Example Qt translation**
```cpp
QToolBar *toolbar = addToolBar(tr("Tabs toolbar"));
```

### Creating a pull-request
For general PRs, you shouldn’t include any updates to the translation source files. They will be updated periodically, primarily around pre-releases, allowing time for any new phrases to be translated before public releases. This is also important in avoiding translation related merge conflicts.

When an updated source file is merged into the GitHub repo, Transifex will automatically detect it (although it can take several hours). Once processed, the new strings will show up as "Remaining" in the Transifex web interface and are ready for translators.

To create the pull-request, use the following commands:
```
git add src/qt/bitcoinstrings.cpp src/qt/locale/bitcoin_en.ts
git commit
```


```xml
<qresource prefix="/translations">
    <file alias="en">locale/bitcoin_en.qm</file>
    ...
</qresource>
```

**Note:** that the language translation file **must end in `.qm`** (the compiled extension), and not `.ts`.
