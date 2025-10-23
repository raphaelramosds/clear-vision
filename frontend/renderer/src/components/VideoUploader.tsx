'use client'

import { useState } from "react";
import { FileMetadata } from "../types";

export default function VideoUploader() {

    const [uploadedFile, setUploadedFile] = useState<FileMetadata>({
        name: '',
        size: 0,
        type: '',
        result: ''
    });

    // see https://stackoverflow.com/questions/46233585/how-to-upload-files-using-react
    const handleFilePicked = (e) => {
        const files = e.target.files;
        const file: File = files[0];

        try {
            if (/\.(mkv|mp4)$/i.test(file.name)) {

                let fileReader = new FileReader();
                fileReader.onloadend = function (event) {
                    console.log("File sent");

                    setUploadedFile({
                        name: file.name,
                        size: file.size / 1000,
                        type: file.type,
                        result: fileReader.result
                    });
                }
                fileReader.readAsDataURL(file);

            } else {
                throw new Error("File should be a video (mkv or mp4)");
            }
        } catch (e) {
            console.error(e);
        }

    }

    return (
        <>
            <input type="file" onChange={handleFilePicked} />

            <p>Informações do arquivo:</p>
            <ul>
                <li>{uploadedFile.name || 'Sem nome'}</li>
                <li>{uploadedFile.size} KB</li>
                <li>{uploadedFile.type || 'Sem tipo definido'}</li>
            </ul>
        </>
    );
}