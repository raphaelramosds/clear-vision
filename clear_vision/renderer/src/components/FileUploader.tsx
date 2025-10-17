'use client'

import { useState } from "react";
import { FileMetadata } from "../types";

export default function FileUploader() {

    const [uploadedFile, setUploadedFile] = useState<FileMetadata>({
        name: '',
        size: 0,
        type: ''
    });

    // see https://stackoverflow.com/questions/46233585/how-to-upload-files-using-react
    const handleFilePicked = (e) => {
        const files = e.target.files;
        const file: File = files[0];
        setUploadedFile({
            name: file.name,
            size: file.size/1000,
            type: file.type
        });
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